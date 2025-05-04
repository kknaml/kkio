module;

import std;
import kkio.traits;

export module kkio.net.ipaddr;

export namespace kkio::net {

    struct IpV6Addr;

    struct IpAddr;

    struct IpV4Addr {
        uint8_t octets_[4];

        constexpr IpV4Addr(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept : octets_{a, b, c, d} {}
        constexpr explicit IpV4Addr(uint32_t addr) noexcept : octets_ {
            static_cast<uint8_t>((addr >> 24) & 0xFF),
            static_cast<uint8_t>((addr >> 16) & 0xFF),
            static_cast<uint8_t>((addr >> 8) & 0xFF),
            static_cast<uint8_t>(addr & 0xFF)
        } {}

        [[nodiscard]]
        auto isUnspecified() const noexcept -> bool;

        [[nodiscard]]
        auto isLoopback() const noexcept -> bool;

        [[nodiscard]]
        auto isPrivate() const noexcept -> bool;

        [[nodiscard]]
        auto isLinkLocal() const noexcept -> bool;

        [[nodiscard]]
        auto isGlobal() const noexcept -> bool;

        [[nodiscard]]
        auto isShared() const noexcept -> bool;

        [[nodiscard]]
        auto isBenchmarking() const noexcept -> bool;

        [[nodiscard]]
        auto isReserved() const noexcept -> bool;

        [[nodiscard]]
        auto isMulticast() const noexcept -> bool;

        [[nodiscard]]
        auto isBroadcast() const noexcept -> bool;

        [[nodiscard]]
        auto isDocumentation() const noexcept -> bool;

        [[nodiscard]]
        auto toIpV6Compatible() const noexcept -> IpV6Addr;

        [[nodiscard]]
        auto toIpV6Mapped() const noexcept -> IpV6Addr;

        static IpV4Addr BROADCAST;
    };

    inline IpV4Addr IpV4Addr::BROADCAST = IpV4Addr{255, 255, 255, 255};

    struct IpV6Addr {
        uint8_t octets_[16]{};

        IpV6Addr(
            uint8_t a, uint8_t b, uint8_t c, uint8_t d,
            uint8_t e, uint8_t f, uint8_t g, uint8_t h
        ) noexcept : octets_{a, b, c, d, e, f, g, h} {}

        explicit IpV6Addr(std::span<uint8_t, 16> addr) noexcept {
            std::ranges::copy(addr, octets_);
        }

        // TODO


        static IpV6Addr UNSPECIFIED;
    };

    inline IpV6Addr IpV6Addr::UNSPECIFIED = IpV6Addr{0, 0, 0, 0, 0, 0, 0, 0};


    struct IpAddr {
        std::variant<IpV4Addr, IpV6Addr> inner_;

        IpAddr(is_one_of<IpV4Addr, IpV6Addr> auto addr) noexcept : inner_(addr) {}

    };

    auto operator==(const IpV4Addr &lhs, const IpV4Addr &rhs) noexcept -> bool;

    auto operator==(const IpV6Addr &lhs, const IpV6Addr &rhs) noexcept -> bool;

    auto operator==(const IpV4Addr &lhs, const IpV6Addr &rhs) noexcept -> bool;

    auto operator==(const IpAddr &lhs, const IpAddr &rhs) noexcept -> bool;

    auto operator<<(std::ostream &os, const IpV4Addr &addr) -> std::ostream &;

    auto operator<<(std::ostream &os, const IpV6Addr &addr) -> std::ostream &;

    auto operator<<(std::ostream &os, const IpAddr &addr) -> std::ostream &;

} // namespace kkio::net

namespace std {

    using kkio::net::IpV4Addr;
    using kkio::net::IpV6Addr;
    using kkio::net::IpAddr;

    template<>
    struct hash<IpV4Addr> {
        auto operator()(const IpV4Addr &addr) const noexcept -> size_t {
            return std::hash<std::string_view>{}(std::string_view(reinterpret_cast<const char *>(&addr), sizeof(addr)));
        }
    };

    template<>
    struct hash<IpV6Addr> {
        auto operator()(const IpV6Addr &addr) const noexcept -> size_t {
            return std::hash<std::string_view>{}(std::string_view(reinterpret_cast<const char *>(&addr), sizeof(addr)));
        }
    };

    template<>
    struct hash<IpAddr> {
        auto operator()(const IpAddr &addr) const noexcept -> size_t {
            return std::visit([](auto &&arg) { return hash<std::decay_t<decltype(arg)>>{}(arg); }, addr.inner_);
        }
    };

    template<>
    struct formatter<IpV4Addr> {
        constexpr auto parse(format_parse_context &ctx) {
            return ctx.begin();
        }

        auto format(const IpV4Addr &addr, format_context &ctx) const {
            return format_to(ctx.out(), "{}.{}.{}.{}", addr.octets_[0], addr.octets_[1], addr.octets_[2], addr.octets_[3]);
        }
    };

    template<>
    struct formatter<IpV6Addr> {
        constexpr auto parse(format_parse_context &ctx) {
            return ctx.begin();
        }

        auto format(const IpV6Addr &addr, format_context &ctx) const {
            return format_to(ctx.out(), "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                addr.octets_[0], addr.octets_[1], addr.octets_[2], addr.octets_[3],
                addr.octets_[4], addr.octets_[5], addr.octets_[6], addr.octets_[7]);
        }
    };

    template<>
    struct formatter<IpAddr> {
        constexpr auto parse(format_parse_context &ctx) {
            return ctx.begin();
        }

        auto format(const IpAddr &addr, format_context &ctx) const {
            return std::visit([&ctx](auto &&arg) {
                return format_to(ctx.out(), "{}", arg);
            }, addr.inner_);
        }
    };

} // namespace std
