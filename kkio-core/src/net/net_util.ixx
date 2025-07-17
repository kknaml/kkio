module;

import kkio.traits;
import kkio.net.sock_addr;

export module kkio.net.net_util;

export namespace kkio::net {

    enum class NetType {
        TCP, UDP
    };

    auto new_socket(int domain, int type, int protocol = 0) noexcept -> int;

    auto new_tcp_socket(const SocketAddress &socket) noexcept -> int;

    auto new_udp_socket(const SocketAddress &socket) noexcept -> int;

    auto new_raw_socket(const SocketAddress &socket) noexcept -> int;

    auto check_socket_fd_result(int fd) noexcept -> Result<int>;

    auto check_socket_fd_throw(int fd) -> int;
}