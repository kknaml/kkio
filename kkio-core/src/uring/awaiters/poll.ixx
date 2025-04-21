module;

#include <liburing.h>

export module kkio.uring.awaiters:poll;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Poll : BaseUringAwaiter {
        Poll(int fd, short events)
            : BaseUringAwaiter{io_uring_prep_poll_add, fd, events} {}

        auto await_resume() const noexcept -> int {
            return this->data.result_;
        }
    };

} 