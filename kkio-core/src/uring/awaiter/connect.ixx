module;

#include <liburing.h>

import std;
import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:connect;

export namespace kkio::uring {

    struct Connect final : BaseUringOP<Connect> {
        int fd;

        Connect(int fd, sockaddr *addr, socklen_t addrlen)
        : BaseUringOP(io_uring_prep_connect, fd, addr, addrlen), fd(fd) {

        }

        Connect(Connect  &&other) noexcept : BaseUringOP(std::move(other)), fd(other.fd) {
        }

        auto await_resume() const -> int {
            if (this->io_data.io_result == 0) [[likely]] {
                return this->fd;
            }
            throw new std::runtime_error(std::format("Connect failed: {}", get_error_msg(this->io_data.io_result)));
        }
    };
}
