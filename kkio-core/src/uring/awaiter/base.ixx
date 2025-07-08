module;

#include <liburing.h>

import std;
import kkio.traits;
import kkio.coro.base;
import kkio.uring.uring;
import kkio.uring.iodata;

export module kkio.uring.awaiter.base;

using kkio::coro::PhantomAwaiter;

// instantiation exposes TU-local entity
auto io_uring_sqe_set_data_forward(io_uring_sqe *sqe, void *data) noexcept -> void {
    ::io_uring_sqe_set_data(sqe, data);
}

export namespace kkio::uring {


    template<typename OP>
    struct BaseUringOP : PhantomAwaiter<>, NonCopy {
        io_uring_sqe *sqe{nullptr};
        IOData io_data{};

        template<typename F, typename ...Args>
        requires std::is_invocable_v<F, io_uring_sqe *, Args...>
        explicit BaseUringOP(F &&f, Args ...args) {
            auto &ring = Ring::current();
            auto *sqe = ring.get_sqe();
            if (sqe != nullptr) [[likely]] {
                io_uring_sqe_set_data_forward(sqe, &this->io_data);
                f(sqe, args...);
                this->sqe = sqe;
            } else {
                TODO();
            }
#ifdef KKIO_DEBUG
            this->io_data.msg = __PRETTY_FUNCTION__;
#endif

        }

        auto await_ready() const noexcept -> bool {
            return sqe == nullptr;
        }

        template<typename Promise>
        auto await_suspend(std::coroutine_handle<Promise> handle) -> void {
            auto &ring = Ring::current();
            handle.promise().cancel_token->invoke_on_cancellation([&] (auto &&token) {
                // TODO lifecycle
                ring.cancel(token, &this->io_data, 0);
            });
            this->io_data.io_handle = handle;
            ring.submit();
        }

    protected:
        auto get_buffer_result() const noexcept -> BufferResult {
            return {.data = io_data.buffer, .size = io_data.io_result};
        }
    };
}
