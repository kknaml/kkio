module;

#include <liburing.h>

import std;
import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:connect;

export namespace kkio::uring {

    struct Connect final : BaseUringOP<Connect> {

        Connect(int fd, sockaddr *addr, socklen_t addrlen)
        : BaseUringOP(io_uring_prep_connect, fd, addr, addrlen) {

        }

        Connect(Connect  &&other) noexcept : BaseUringOP(std::move(other)) {
        }

        auto await_resume() const noexcept -> int {
            return this->io_data.io_result;
        }
    };
}
