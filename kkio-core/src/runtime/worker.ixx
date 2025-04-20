module;

#include <sys/eventfd.h>
#include <unistd.h>
#include <liburing.h>
#include <cstdio>
#include <memory>
#include <cstdlib>

import std;
import kkio.traits;
import kkio.uring.context;
import kkio.uring.iodata;

export module kkio.runtime.worker;


export namespace kkio::runtime {

    class StealingWorker : NonCopy {
    private:
        std::queue<std::coroutine_handle<>> tasks_{};
        mutable std::mutex queue_mutex_{};
        int event_fd_{-1};
        size_t index_;
        bool stop_{false};
        io_uring *ring_{nullptr};
        io_uring_buf_ring *br_{nullptr};
        void **bufs_{nullptr};
        uint64_t event_buf_{0};

    public:
        explicit StealingWorker(size_t index) : index_(index) {
            workers_.emplace_back(this);
            event_fd_ = eventfd(0, EFD_NONBLOCK);
            if (event_fd_ == -1) {
                throw std::runtime_error("Failed to create eventfd");
            }
        }

        ~StealingWorker() {
            if (event_fd_ != -1) {
                close(event_fd_);
            }
            if (ring_) {
                io_uring_queue_exit(ring_);
                delete ring_;
            }
            // TODO remove this from workers_
        }

        auto addTask(std::coroutine_handle<> task) -> void {
            {
                std::lock_guard lock(queue_mutex_);
                tasks_.push(task);
            }
            writeEventFd();
        }

        auto queueSize() const noexcept -> size_t {
            std::lock_guard lock(queue_mutex_);
            return tasks_.size();
        }

        auto run() noexcept -> void {
            setupRing();
            setupBufferRing();

            while (!stop_) {
                io_uring_cqe *cqe;
                int ret = io_uring_wait_cqe(ring_, &cqe);
                if (ret < 0) {
                    std::println("ret < 0");
                    continue;
                }

                if (cqe->user_data == reinterpret_cast<uint64_t>(this)) [[unlikely]] {
                    std::println("handle event");
                    processNewTasks();
                    prepEventRead();
                } else [[likely]] {
                    std::println("handle completion");
                    handleCompletion(cqe);
                }
                io_uring_cqe_seen(ring_, cqe);
            }
        }

    private:
        auto setupRing() noexcept -> void {

            auto ringWrapper = new uring::Ring;
            uring::setLocalRing(ringWrapper);
            this->ring_ = ringWrapper->inner();

            // TODO handle error
            io_uring_queue_init(32, ring_, 0);
            prepEventRead();
        }

        auto setupBufferRing() noexcept -> void {
            io_uring_buf_reg reg{};
            // TODO
            uint32_t bufferInGroud = 128;
            size_t bufferSize = 4096;
            size_t pageSize = sysconf(_SC_PAGE_SIZE);
            std::println("Setting up buffer ring with {} buffers of size {}", bufferInGroud, bufferSize);
            bufs_ = new void*[bufferInGroud];
            for (size_t i = 0; i < bufferInGroud; i++) {
                if (posix_memalign(&bufs_[i], pageSize, bufferSize)) [[unlikely]] {
                    std::println(stderr, "Failed to allocate buffer {}", i);
                    std::abort();
                }
            }
            if (posix_memalign(reinterpret_cast<void **>(&br_), pageSize, bufferInGroud * sizeof(io_uring_buf_ring))) [[unlikely]] {
                std::println(stderr, "Failed to allocate buffer ring");
                std::abort();
            }
            reg.ring_addr = reinterpret_cast<uint64_t>(br_);
            reg.ring_entries = bufferInGroud;
            reg.bgid = 1;
            if (auto r = io_uring_register_buf_ring(ring_, &reg, 0); r) [[unlikely]] {
                std::println(stderr, "Failed to register buffer ring {}, errno: {}", r, errno);
                std::abort();
            }
            std::println("Buffer ring registered successfully");
            io_uring_buf_ring_init(br_);
            for (int i = 0; i < bufferInGroud; i++) {
                io_uring_buf_ring_add(br_, bufs_[i], bufferSize, i,
                    io_uring_buf_ring_mask(bufferInGroud), i);
            }
            io_uring_buf_ring_advance(br_, bufferInGroud);
            std::println("Buffer ring setup completed");
        }

        auto prepEventRead() -> void {
            io_uring_sqe* sqe = io_uring_get_sqe(ring_);
            io_uring_prep_read(sqe, event_fd_, &event_buf_, sizeof(event_buf_), 0);
            io_uring_sqe_set_data(sqe, this);
            io_uring_submit(ring_);
        }

        auto writeEventFd() -> void {
            // io_uring_sqe *sqe = io_uring_get_sqe(ring_);
            // if (!sqe) {
            //     // TODO
            // }
            uint64_t value = 1;
            // io_uring_prep_write(sqe, event_fd_, &value, sizeof(value), 0);
            // io_uring_sqe_set_data(sqe, nullptr);
            // io_uring_submit(ring_);
            write(event_fd_, &value, sizeof(value));
        }

        auto processNewTasks() noexcept -> void {
            std::unique_lock lock(queue_mutex_);
            while (!tasks_.empty()) {
                auto task = tasks_.front();
                tasks_.pop();
                lock.unlock();
                resumeHandle(task);
                lock.lock();
            }
        }

        auto handleCompletion(io_uring_cqe *cqe) noexcept -> void {
            auto data = reinterpret_cast<uring::IOData *>(cqe->user_data);
            if (data == nullptr) [[unlikely]] {
                std::println(stderr, "ioda null, res: {}", cqe->res);
                return;
            }
            auto res = cqe->res;
            data->result_ = res;
            if (res <= 0) [[unlikely]] {
                if (res == -EFAULT) {
                    std::println(stderr, "EFAULT error occurred, flags: {}, user_data: {}", cqe->flags, cqe->user_data);
                }
                std::println(stderr, "cqe res: {}, flags: {}", cqe->res, cqe->flags);
                resumeHandle(data->handle_);
                return;
            }
            if (cqe->flags & IORING_CQE_F_BUFFER) {
                uint16_t bid = cqe->flags >> IORING_CQE_BUFFER_SHIFT;
                std::println("bid: {}, buffer: {}", bid, (void *)bufs_[bid]);
                auto buf = bufs_[bid];
                data->buf_ = buf;
                resumeHandle(data->handle_);
                io_uring_buf_ring_add(br_, buf, 4096, bid,
                    io_uring_buf_ring_mask(128), 0);
                io_uring_buf_ring_advance(br_, 1);
                return;
            }
            resumeHandle(data->handle_);
        }

        static auto resumeHandle(std::coroutine_handle<> handle) -> void {
            try {
                handle();
            } catch (std::exception &e) {
                std::println(stderr, "error in task: {}", e.what());
            } catch (...) {
                std::println(stderr, "unknown error in task");
            }
        }

        static std::vector<StealingWorker*> workers_;
    };

    class WorkerManager : NonCopy {
    private:
        struct WorkerHolder {
            StealingWorker *worker_;
            std::thread thread_;
        };
        std::vector<WorkerHolder> workers_{};
        std::mutex lock_{};
        std::condition_variable cv_{};
    public:
        explicit WorkerManager(size_t workerSize) {
            workers_.reserve(workerSize);
            for (int i = 0; i < workerSize; i++) {
                auto worker = new StealingWorker(i);
                workers_.emplace_back(worker, std::thread([worker] {
                    worker->run();
                }));
            }
        }

        auto addTask(std::coroutine_handle<> task) -> void {
            auto rand = std::rand() % workers_.size();
            workers_[rand].worker_->addTask(task);
        }

        auto wait() -> void {
            std::unique_lock lock(lock_);
            cv_.wait(lock);
        }
    };

}
