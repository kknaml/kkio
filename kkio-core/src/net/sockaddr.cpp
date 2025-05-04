module kkio.net.sockaddr;

namespace kkio::net {

    SocketAddr::SocketAddr(const IpAddr &ip, uint16_t port) noexcept : inner_(
    std::visit(Overloads {
        [port] (const IpV4Addr &v4) -> std::variant<SocketAddrV4, SocketAddrV6> {
            return SocketAddrV4(v4, port);
        },
        [port] (const IpV6Addr &v6) -> std::variant<SocketAddrV4, SocketAddrV6> {
            return SocketAddrV6(v6, port, 0, 0);
        }
    }, ip.inner_)
    ){}

    auto SocketAddr::ip() const noexcept -> IpAddr {
        return std::visit(Overloads {
            [] (const SocketAddrV4 &v4) -> IpAddr {
                return IpAddr{v4.ip_};
            },
            [] (const SocketAddrV6 &v6) -> IpAddr {
                return IpAddr{v6.ip_};
            }
        }, inner_);
    }

    auto SocketAddr::port() const noexcept -> uint16_t {
        return std::visit(Overloads {
            [] (const SocketAddrV4 &v4) {
             return v4.port_;
         },
         [] (const SocketAddrV6 &v6) {
             return v6.port_;
        }}, inner_);
    }

    auto SocketAddr::isV4() const noexcept -> bool {
        return std::holds_alternative<SocketAddrV4>(inner_);
    }

    auto SocketAddr::isV6() const noexcept -> bool {
        return std::holds_alternative<SocketAddrV6>(inner_);
    }
}
