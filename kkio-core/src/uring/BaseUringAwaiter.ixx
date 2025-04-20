module;


#include<liburing.h>


import std;
import kkio.traits;
import kkio.coro_base;
import kkio.uring.iodata;
import kkio.uring.context;

export module kkio.uring.base_awaiter;


export namespace kkio::uring {

    struct BaseUringAwaiter : coro::AwaiterMarker, NonCopy {
        io_uring_sqe *sqe_{nullptr};
        IOData data{};

        template<typename F, typename ...Args>
        requires std::invocable<F, io_uring_sqe *, Args...>
        explicit BaseUringAwaiter(F &&f, Args ...args) {
#ifdef DEBUG
            if (getLocalRing() == nullptr) {
                throw std::runtime_error("localRingContext is null");
            }
#endif

            auto ring = getLocalRing();
            this->sqe_ = ring->getSqe();
            if (sqe_ != nullptr) [[likely]] {
                f(sqe_, args...);
                io_uring_sqe_set_data(sqe_, &this->data);
            } else {
             // TODO
                std::abort();
            }
        }

        auto await_ready() const noexcept -> bool {
             return sqe_ == nullptr;
         }

        auto await_suspend(std::coroutine_handle<> handle) noexcept -> void {
             data.handle_ = handle;
             auto ring = getLocalRing();
             ring->submit();
         }

        auto onCancel() noexcept -> void {
            if (sqe_ != nullptr) [[likely]] {
                io_uring_prep_cancel(sqe_, &this->data, 0);
                io_uring_sqe_set_data(sqe_, nullptr);
            }
        }

    protected:
        auto setBufferSelectFlag() -> void {
            std::println("Setting flags: IOSQE_BUFFER_SELECT");
            this->sqe_->flags |= IOSQE_BUFFER_SELECT;
            std::println("Setting buf_group to 1");
            this->sqe_->buf_group = 1;
            std::println("SQE flags after setup: {}", this->sqe_->flags);
        }
    };

} // namespace kkio::uring
