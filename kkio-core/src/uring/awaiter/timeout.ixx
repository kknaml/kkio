module;

#include <liburing.h>

import std;
import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;
import kkio.coro.base;

export module kkio.uring.awaiter.all:timeout;

export namespace kkio::uring {

    struct Timeout final : BaseUringOP<Timeout> {
        __kernel_timespec ts{};

        Timeout(__kernel_timespec ts, uint32_t count, uint32_t flags)
        : BaseUringOP(io_uring_prep_timeout, &this->ts, count, flags), ts(ts) {

        }

        auto await_resume() const noexcept -> void {

        }
    };

    auto delay(long mills) -> Timeout {
        __kernel_timespec ts{};
        ts.tv_sec = mills / 1000;
        ts.tv_nsec = (mills % 1000) * 1000000;
        return Timeout(ts, 1, IORING_TIMEOUT_ETIME_SUCCESS);
    }

}

export namespace kkio::coro {

    template<>
    struct PhantomAwaiter<std::chrono::microseconds> {
        static auto await_transform(const std::chrono::microseconds &micro) -> decltype(auto) {
            __kernel_timespec ts{};
            ts.tv_nsec = micro.count() * 1000;
            return uring::Timeout(ts, 1, IORING_TIMEOUT_ETIME_SUCCESS);
        }
    };

    template<>
    struct PhantomAwaiter<std::chrono::milliseconds> {
        static auto await_transform(const std::chrono::milliseconds &ms) -> decltype(auto) {
            return uring::delay(ms.count());
        }
    };

    template<>
    struct PhantomAwaiter<std::chrono::seconds> {
        static auto await_transform(const std::chrono::seconds &t) -> decltype(auto) {
            // TODO
            return uring::delay(t.count() * 1000);
        }
    };

    template<>
    struct PhantomAwaiter<std::chrono::minutes> {
        static auto await_transform(const std::chrono::minutes &m) -> decltype(auto) {
            return uring::delay(m.count() * 60 * 1000);
        }
    };
}


