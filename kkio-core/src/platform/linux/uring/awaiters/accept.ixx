
module;

#include <liburing.h>

export module kkio.uring.awaiters:accept;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Accept : BaseUringAwaiter {

        Accept(int fd, sockaddr *addr, socklen_t *addrlen, int flags)
            : BaseUringAwaiter{io_uring_prep_accept, fd, addr, addrlen, flags} {}

        auto await_resume() const noexcept -> int {
            return this->data.result_;
        }
    };

}
