module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:accept;

export namespace kkio::uring {

    struct Accept final : BaseUringOP<Accept> {

        Accept(int fd, sockaddr *addr, socklen_t *addrlen, int flags)
        : BaseUringOP(io_uring_prep_accept, fd, addr, addrlen, flags) {

        }

        auto await_resume() const noexcept -> int {
            return this->io_data.io_result;
        }
    };
}
