module;

#include <sys/socket.h>

import std;
import kkio.traits;
import kkio.net.sock_addr;

module kkio.net.net_util;

namespace kkio::net {

    auto new_socket(int domain, int type, int protocol) noexcept -> int {
        return socket(domain, type, protocol);
    }

    auto new_tcp_socket(const SocketAddress &socket) noexcept -> int {
        int domain = socket.family();
        return new_socket(domain, SOCK_STREAM, 0);
    }

    auto new_udp_socket(const SocketAddress &socket) noexcept -> int {
        int domain = socket.family();
        return new_socket(domain, SOCK_DGRAM, 0);
    }

    auto new_raw_socket(const SocketAddress &socket) noexcept -> int {
        int domain = socket.family();
        return new_socket(domain, SOCK_RAW, 0);
    }

    auto check_socket_fd_result(int fd) noexcept -> Result<int> {
        if (fd >= 0) return fd;
        auto code = -fd;
        return std::unexpected(Error::from(code));
    }

    auto check_socket_fd_throw(int fd) -> int {
        if (fd >= 0) return fd;
        auto code = -fd;
        auto msg = get_error_msg(code);
        throw std::runtime_error(std::format("Socket create error code: {}, msg: {}", code, msg));
    }
}
