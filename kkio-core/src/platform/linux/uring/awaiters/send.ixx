module;

#include <liburing.h>

export module kkio.uring.awaiters:send;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Send : BaseUringAwaiter {
        Send(int fd, const void *buf, size_t len, int flags)
            : BaseUringAwaiter{io_uring_prep_send, fd, buf, len, flags} {}

        auto await_resume() const noexcept -> ssize_t {
            return this->data.result_;
        }
    };

} 