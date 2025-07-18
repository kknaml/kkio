module;

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

import std;

module kkio.net.sock_addr;

namespace kkio::net {

    auto SocketAddress::from_string(std::string_view address) -> std::unique_ptr<SocketAddress> {
        if (address.contains('/')) {
            return std::make_unique<UnixAddr>(address);
        }

        // 192.168.1.1:8080 or [::1]:8080
        auto colon_pos = address.rfind(':');
        if (colon_pos == std::string_view::npos) {
            throw std::invalid_argument(std::format("Invalid address format: {}", address));
        }

        auto host = std::string(address.substr(0, colon_pos));
        auto port = static_cast<uint16_t>(std::stoi(address.substr(colon_pos + 1).data()));
        return from_string(host, port);
    }

    auto SocketAddress::from_string(std::string_view host, uint16_t port) -> std::unique_ptr<SocketAddress> {
        {
            sockaddr_in v4addr{};
            if (inet_pton(AF_INET, host.data(), &v4addr.sin_addr) == 1) {
                v4addr.sin_family = AF_INET;
                v4addr.sin_port = htons(port);
                return std::make_unique<IPv4Addr>(v4addr);
            }
        }

        {
            sockaddr_in6 v6addr{};
            if (inet_pton(AF_INET6, host.data(), &v6addr.sin6_addr) == 1) {
                v6addr.sin6_family = AF_INET6;
                v6addr.sin6_port = htons(port);
                return std::make_unique<IPv6Addr>(v6addr);
            }
        }

        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        addrinfo *result{nullptr};
        if (getaddrinfo(host.data(), nullptr, &hints, &result) != 0) {
            throw std::invalid_argument(std::format("Invalid address format: {}", host));
        }

        std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> guard(result, freeaddrinfo);

        if (result->ai_family == AF_INET) {
            auto *v4 = reinterpret_cast<sockaddr_in *>(result->ai_addr);
            v4->sin_port = htons(port);
            return std::make_unique<IPv4Addr>(*v4);
        } else if (result->ai_family == AF_INET6) {
            auto *v6 = reinterpret_cast<sockaddr_in6 *>(result->ai_addr);
            v6->sin6_port = htons(port);
            return std::make_unique<IPv6Addr>(*v6);
        }

        throw std::invalid_argument(std::format("Invalid address format: {}", host));
    }

    IPv4Addr::IPv4Addr(std::string_view ip, uint16_t port) : addr({}) {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.data(), &addr.sin_addr) != 1) {
            throw std::invalid_argument(std::format("Invalid address format: {}", ip));
        }
    }

    IPv4Addr::IPv4Addr(const sockaddr_in &addr) : addr(addr) {

    }

    auto IPv4Addr::to_string() const -> std::string {
        char ip_str[INET_ADDRSTRLEN];
        const char* ret = inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));
        if (ret == nullptr) {
            throw std::runtime_error("inet_ntop failed");
        }
        return std::string(ip_str) + ":" + std::to_string(ntohs(addr.sin_port));
    }

    IPv6Addr::IPv6Addr(std::string_view ip, uint16_t port) : addr({}) {
        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(port);

        if (ip.front() == '[' && ip.back() == ']') {
            ip = ip.substr(1, ip.size() - 2); // 去除 "[...]" 包裹
        }
        if (inet_pton(AF_INET6, ip.data(), &addr.sin6_addr) != 1) {
            throw std::invalid_argument("Invalid IPv6 address: " + std::string(ip));
        }
    }

    IPv6Addr::IPv6Addr(const sockaddr_in6 &addr) : addr(addr) {

    }

    auto IPv6Addr::to_string() const -> std::string {
        char ip_str[INET6_ADDRSTRLEN];
        const char* ret = inet_ntop(AF_INET6, &addr.sin6_addr, ip_str, sizeof(ip_str));
        if (ret == nullptr) {
            throw std::runtime_error("inet_ntop failed");
        }
        return "[" + std::string(ip_str) + "]:" + std::to_string(ntohs(addr.sin6_port));
    }


    UnixAddr::UnixAddr(std::string_view path) : addr({}), path(path) {
        if (path.size() >= sizeof(addr.sun_path)) {
            throw std::invalid_argument("Unix socket path too long");
        }
        addr.sun_family = AF_UNIX;
        std::memcpy(addr.sun_path, path.data(), path.size());
    }

    UnixAddr::UnixAddr(const sockaddr_un &addr) : addr(addr), path(addr.sun_path) {

    }

    auto UnixAddr::to_string() const -> std::string {
        return path;
    }

}
