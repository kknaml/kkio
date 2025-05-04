module;

#include <liburing.h>

export module kkio.uring.awaiters:connect;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Connect : BaseUringAwaiter {
        Connect(int fd, const sockaddr *addr, socklen_t addrlen)
            : BaseUringAwaiter{io_uring_prep_connect, fd, addr, addrlen} {}

        auto await_resume() const noexcept -> int {
            return this->data.result_;
        }
    };

} 