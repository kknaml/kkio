module;

#include <liburing.h>

export module kkio.uring.awaiters:recv;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Recv : BaseUringAwaiter {
        Recv(int fd, void *buf, size_t len, int flags)
            : BaseUringAwaiter{io_uring_prep_recv, fd, buf, len, flags} {}

        auto await_resume() const noexcept -> ssize_t {
            return this->data.result_;
        }
    };

} 