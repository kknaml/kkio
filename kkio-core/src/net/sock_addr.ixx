module;

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

import std;
import kkio.traits;

export module kkio.net.sock_addr;

export namespace kkio::net {

    class SocketAddress {
    public:
        virtual ~SocketAddress() = default;

        virtual auto family() const noexcept -> sa_family_t = 0;

        virtual auto to_addr() const noexcept -> const sockaddr * = 0;

        virtual auto to_addr() noexcept -> sockaddr * = 0;

        virtual auto get_port() const noexcept -> uint16_t = 0;

        virtual auto length() const noexcept -> socklen_t = 0;

        virtual auto to_string() const -> std::string = 0;

        static auto from_string(std::string_view address) -> std::unique_ptr<SocketAddress>;

        static auto from_string(std::string_view host, uint16_t port) -> std::unique_ptr<SocketAddress>;
    };

    class IPv4Addr final : public  SocketAddress {
    private:
        sockaddr_in addr;
    public:
        IPv4Addr(std::string_view ip, uint16_t port);
        explicit IPv4Addr(const sockaddr_in &addr);

        auto family() const noexcept -> sa_family_t override {
            return AF_INET;
        }

        auto to_addr() const noexcept -> const sockaddr * override {
            return reinterpret_cast<const sockaddr *>(&addr);
        }

        auto to_addr() noexcept -> sockaddr * override {
            return reinterpret_cast<sockaddr *>(&addr);
        }

        auto get_port() const noexcept -> uint16_t override {
            return ntohs(addr.sin_port);
        }

        constexpr auto length() const noexcept -> socklen_t override {
            return sizeof(this->addr);
        }

        auto to_string() const -> std::string override;
    };

    class IPv6Addr final : public  SocketAddress {
    private:
        sockaddr_in6 addr;
    public:
        IPv6Addr(std::string_view ip, uint16_t port);
        explicit IPv6Addr(const sockaddr_in6 &addr);

        auto family() const noexcept -> sa_family_t override {
            return AF_INET6;
        }

        auto to_addr() noexcept -> sockaddr * override {
            return reinterpret_cast<sockaddr *>(&addr);
        }

        auto to_addr() const noexcept -> const sockaddr * override {
            return reinterpret_cast<const sockaddr *>(&addr);
        }

        auto get_port() const noexcept -> uint16_t override {
            return addr.sin6_port;
        }

        constexpr auto length() const noexcept -> socklen_t override {
            return sizeof(this->addr);
        }

        auto to_string() const -> std::string override;
    };

    class UnixAddr final : public  SocketAddress {
    private:
        sockaddr_un addr;
        std::string path;
    public:
        explicit UnixAddr(std::string_view path);
        explicit UnixAddr(const sockaddr_un &addr);

        auto family() const noexcept -> sa_family_t override {
            return AF_UNIX;
        }

        auto to_addr() noexcept -> sockaddr * override {
            return reinterpret_cast<sockaddr *>(&addr);
        }

        auto to_addr() const noexcept -> const sockaddr * override {
            return reinterpret_cast<const sockaddr *>(&addr);
        }

        auto get_port() const noexcept -> uint16_t override {
            return ntohs(addr.sun_family);
        }

        auto length() const noexcept -> socklen_t override {
            return sizeof(this->addr) + path.length();
        }

        auto to_string() const -> std::string override;
    };

}
