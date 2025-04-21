module;

#include <cstdint>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

import std;

export module kkio.net.address;

export namespace kkio::net {

    class Endpoint {
    private:
        union {
            sockaddr_in v4;
            sockaddr_in6 v6;
        } addr_{};
        socklen_t len_{};

    public:
        Endpoint() = default;

        Endpoint(std::string_view ip, uint16_t port) {
            addr_.v4.sin_family = AF_INET;
            addr_.v4.sin_port = htons(port);
            if (inet_pton(AF_INET, ip.data(), &addr_.v4.sin_addr) != 1) {
                throw std::runtime_error("Invalid IPv4 address");
            }
            len_ = sizeof(sockaddr_in);
        }

        Endpoint(std::string_view ip, uint16_t port, uint32_t flowinfo = 0, uint32_t scope_id = 0) {
            addr_.v6.sin6_family = AF_INET6;
            addr_.v6.sin6_port = htons(port);
            addr_.v6.sin6_flowinfo = flowinfo;
            addr_.v6.sin6_scope_id = scope_id;
            if (inet_pton(AF_INET6, ip.data(), &addr_.v6.sin6_addr) != 1) {
                throw std::runtime_error("Invalid IPv6 address");
            }
            len_ = sizeof(sockaddr_in6);
        }


        Endpoint(const sockaddr* addr, socklen_t len) {
            if (len > sizeof(addr_)) {
                throw std::runtime_error("Address too large");
            }
            std::memcpy(&addr_, addr, len);
            len_ = len;
        }

        auto family() const noexcept -> int {
            return addr_.v4.sin_family;
        }

        auto isV4() const noexcept -> bool {
            return family() == AF_INET;
        }

        auto isV6() const noexcept -> bool {
            return family() == AF_INET6;
        }

        auto port() const noexcept -> uint16_t {
            return isV4() ? ntohs(addr_.v4.sin_port) : ntohs(addr_.v6.sin6_port);
        }

        auto toString() const -> std::string {
            char buf[INET6_ADDRSTRLEN];
            const char* ip_str;
            if (isV4()) {
                ip_str = inet_ntop(AF_INET, &addr_.v4.sin_addr, buf, sizeof(buf));
            } else {
                ip_str = inet_ntop(AF_INET6, &addr_.v6.sin6_addr, buf, sizeof(buf));
            }
            if (!ip_str) {
                throw std::runtime_error("Failed to convert address to string");
            }
            return std::format("{}:{}", ip_str, port());
        }

        auto sockaddr() const noexcept -> const sockaddr* {
            return reinterpret_cast<const struct sockaddr*>(&addr_);
        }

        auto socklen() const noexcept -> socklen_t {
            return len_;
        }
    };

} // namespace kkio::net 