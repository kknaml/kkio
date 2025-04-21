module;

#include <liburing.h>

import kkio.uring.awaiters;
import kkio.coro_base;

export module kkio.util.delay;

export namespace kkio::util {


    namespace detail {
        struct DelayAwaiter : uring::Timeout {

            __kernel_timespec ts_{};

            explicit DelayAwaiter(long ms) noexcept : Timeout(&ts_, 0, IORING_TIMEOUT_ETIME_SUCCESS) {
                if (ms < 0) [[unlikely]] { // still suspend
                    ms = 0;
                }
                ts_.tv_sec = ms / 1000;
                ts_.tv_nsec = (ms % 1000) * 1000000;
            }

            auto await_resume() const -> void {
                auto result = Timeout::await_resume();
                if (result == - ECANCELED) [[unlikely]] {

                }
            }
        };
    } // namespace detail

    auto delay(long ms) -> coro::Awaitable<void> auto {
        return detail::DelayAwaiter{ms};
    }

} // namespace kkio::util
