module;

#include <liburing.h>

export module kkio.uring.awaiters:timeout;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Timeout : BaseUringAwaiter {
        Timeout(__kernel_timespec *ts, unsigned count, unsigned flags)
            : BaseUringAwaiter{io_uring_prep_timeout, ts, count, flags} {}

        auto await_resume() const noexcept -> int {
            return this->data.result_;
        }
    };

} 