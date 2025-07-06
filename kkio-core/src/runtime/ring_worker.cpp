module;

#include <liburing.h>
#include <sys/eventfd.h>

import std;
import kkio.traits;
import kkio.uring.iodata;

module kkio.runtime.worker;

using kkio::uring::IOData;

namespace {

    using kkio::runtime::RingWorker;

    auto resume_handle(std::coroutine_handle<> handle) noexcept {
        try {
            handle();
        } catch (...) {
            std::println("resume handle error");
            std::abort();
        }
    }

    auto wake_up(int fd) -> void {
        uint64_t data = 1;
        write(fd, &data, sizeof(uint64_t));
    }

    auto prep_event_read(int fd, RingWorker &worker, uint64_t *buf) {
        auto &ring = worker.get_ring();
        auto sqe = ring.get_sqe();
        sqe->user_data = reinterpret_cast<uint64_t>(&worker);
        io_uring_prep_read(sqe, fd, buf, sizeof(*buf), 0);
        ring.submit();
    }
}

namespace kkio::runtime {

    RingWorker::RingWorker(uint32_t entries, uint32_t flag, uint32_t bufs_in_group) {
        this->ring.init(entries, flag, bufs_in_group);
        this->thread = std::thread(&RingWorker::run, this);
        this->event_fd = eventfd(1, 0);
    }

    auto RingWorker::run() noexcept -> void {
        while (this->running) {

            { // app handle
                auto handle = this->get_handle();
                if (handle) {
                    resume_handle(handle);
                    continue;
                }
            }

            io_uring_cqe *cqe{};
            auto ret = this->ring.wait(&cqe);
            if (ret == 0) [[likely]] {
                auto user_data = cqe->user_data;
                if (user_data == reinterpret_cast<uint64_t>(this)) { // event fd
                    io_uring_cqe_seen(ring.get_ring(), cqe);
                    continue;
                }

                auto data = reinterpret_cast<IOData *>(user_data);
                data->io_result = cqe->res;
                if (cqe->flags & IORING_CQE_F_BUFFER) {
                    auto buffer_id = cqe->flags >> IORING_CQE_BUFFER_SHIFT;
                    data->buffer = this->ring.get_buffer(buffer_id);
                }
                resume_handle(data->io_handle);
            } else {
                std::abort();
            }
        }
    }

    auto RingWorker::get_handle() noexcept -> std::coroutine_handle<> {
        std::lock_guard guard(this->mutex);
        if (queue.empty()) return nullptr;
        auto handle =  queue.front();
        queue.pop();
        return handle;
    }

    RingWorker::~RingWorker() noexcept {
        this->running = false;
        wake_up(this->event_fd);
        if (this->thread.joinable()) this->thread.join();
        close(this->event_fd);
    }

    auto RingWorker::get_ring() noexcept -> Ring & {
        return this->ring;
    }

    auto RingWorker::add_handle(std::coroutine_handle<> handle) -> void {
        std::lock_guard lock(this->mutex);
        this->queue.push(handle);
    }

    auto RingWorker::notify() noexcept -> void {
        std::lock_guard lock(this->mutex);
        if (this->queue.empty()) return;
        wake_up(this->event_fd);
    }

    auto RingWorker::create(uint32_t entries, uint32_t flag, uint32_t bufs_in_group) -> std::unique_ptr<RingWorker> {
        return std::make_unique<RingWorker>(entries, flag, bufs_in_group);
    }
}
