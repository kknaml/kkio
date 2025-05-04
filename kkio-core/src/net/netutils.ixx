
module;

import std;

// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <cerrno>

export module kkio.net.netutils;

export namespace kkio::net {

    // inline auto closeSocket(int fd) noexcept -> int {
    //     return close(fd);
    // }
    //
    //
    // inline auto createServerSocket(std::string_view host, int port) -> int {
    //     auto fd = socket(AF_INET, SOCK_STREAM, 0);
    //     if (fd < 0) {
    //         throw std::runtime_error(std::format("create socket failed: {}", fd));
    //     }
    //     int opt = 1;
    //     if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    //         closeSocket(fd);
    //         throw std::runtime_error(std::format("set sockopt failed: {}", opt));
    //     }
    //     sockaddr_in addr{};
    //     addr.sin_family = AF_INET;
    //     addr.sin_port = htons(port);
    //     // addr.sin_addr.s_addr = inet_addr(host.data());
    //     if (inet_pton(AF_INET, host.data(), &addr.sin_addr) <= 0) {
    //         closeSocket(fd);
    //         std::println("invalid host: {}, err: {}", host, errno);
    //     }
    //     if (auto r = bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)); r < 0) {
    //         closeSocket(fd);
    //         std::println("bind socket failed: {}", errno);
    //         throw std::runtime_error(std::format("bind socket failed: {}", errno));
    //     }
    //     if (listen(fd, SOMAXCONN) < 0) {
    //         closeSocket(fd);
    //         throw std::runtime_error(std::format("listen socket failed: {}", fd));
    //     }
    //     return fd;
    //
    // }

}
