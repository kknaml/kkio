
export module kkio.net.sockaddr;

import std;
import kkio.net.ipaddr;
import kkio.traits;

export namespace kkio::net {

    struct SocketAddrV6;

    struct SocketAddrV4 {
        IpV4Addr ip_;
        uint16_t port_;

        SocketAddrV4(IpV4Addr ip, uint16_t port) noexcept : ip_(ip), port_(port) {}
    };

    struct SocketAddrV6 {
        IpV6Addr ip_;
        uint16_t port_;
        uint32_t flowInfo_;
        uint32_t scopeId_;

        SocketAddrV6(IpV6Addr ip, uint16_t port, uint32_t flowInfo, uint32_t scopeId) noexcept
            : ip_(ip), port_(port), flowInfo_(flowInfo), scopeId_(scopeId) {}
    };

    struct SocketAddr {
        std::variant<SocketAddrV4, SocketAddrV6> inner_;
        SocketAddr(is_one_of<SocketAddrV4, SocketAddrV6> auto &&addr) noexcept : inner_(addr) {}

        SocketAddr(const IpAddr &ip, uint16_t port) noexcept;

        [[nodiscard]]
        auto ip() const noexcept -> IpAddr;

        [[nodiscard]]
        auto port() const noexcept -> uint16_t;

        [[nodiscard]]
        auto isV4() const noexcept -> bool;

        [[nodiscard]]
        auto isV6() const noexcept -> bool;

    };
}

template<>
struct std::formatter<kkio::net::SocketAddr> {
    constexpr auto parse(auto &&ctx) {
        return ctx.begin();
    }

    auto format(const kkio::net::SocketAddr &addr, auto &&ctx) const {
        return std::format_to(ctx.out(), "{}:{}", addr.ip(), addr.port());
    }
};

