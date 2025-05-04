module kkio.net.ipaddr;

import std;
import kkio.traits;


namespace kkio::net {

    namespace {
        auto bytes2U32(const uint8_t(&arr)[4]) noexcept -> uint32_t {
            uint32_t ret{};
            std::copy_n(arr, 4, &ret);
            return ret;
        }
    }


    auto IpV4Addr::isUnspecified() const noexcept -> bool {
        return bytes2U32(octets_) == 0;
    }

    auto IpV4Addr::isLoopback() const noexcept -> bool {
        return octets_[0] == 127;
    }

    auto IpV4Addr::isPrivate() const noexcept -> bool {
        if (octets_[0] == 10) {
            return true;
        }
        if (octets_[0] == 172 && octets_[1] >= 16 && octets_[1] <= 31) {
            return true;
        }
        if (octets_[0] == 192 && octets_[1] == 168) {
            return true;
        }
        return false;
    }

    auto IpV4Addr::isLinkLocal() const noexcept -> bool {
        return octets_[0] == 169 && octets_[1] == 254;
    }

    auto IpV4Addr::isGlobal() const noexcept -> bool {
        // TODO
        return false;
    }

    auto IpV4Addr::isShared() const noexcept -> bool {
        return octets_[0] == 100 && (octets_[1] & 0b11000000 == 0b01000000);
    }

    auto IpV4Addr::isBenchmarking() const noexcept -> bool {
        return octets_[0] == 198 && (octets_[1] & 0xfe) == 18;
    }

    auto IpV4Addr::isReserved() const noexcept -> bool {
        return octets_[0] == 240 && !isBroadcast();;
    }

    auto IpV4Addr::isMulticast() const noexcept -> bool {
        return octets_[0] >= 224 && octets_[0] <= 239;
    }

    auto IpV4Addr::isBroadcast() const noexcept -> bool {
        return *reinterpret_cast<const uint32_t*>(octets_) == *reinterpret_cast<const uint32_t*>(BROADCAST.octets_);
    }

    auto IpV4Addr::isDocumentation() const noexcept -> bool {
        return (octets_[0] == 192 && octets_[1] == 0 && octets_[2] == 2) ||
               (octets_[0] == 198 && octets_[1] == 51 && octets_[2] == 100) ||
               (octets_[0] == 203 && octets_[1] == 0 && octets_[2] == 113);
    }

    auto IpV4Addr::toIpV6Compatible() const noexcept -> IpV6Addr {
        auto [a, b, c, d] = octets_;
        uint8_t args[16] = {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            a, b, c, d
        };
        return IpV6Addr(args);
    }

    auto IpV4Addr::toIpV6Mapped() const noexcept -> IpV6Addr {
        auto [a, b, c, d] = octets_;
        uint8_t args[16] = {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0xff, 0xff,
            a, b, c, d
        };
        return IpV6Addr(args);
    }



    auto operator==(const IpV4Addr &lhs, const IpV4Addr &rhs) noexcept -> bool {
        return std::equal(std::begin(lhs.octets_), std::end(lhs.octets_), std::begin(rhs.octets_));
    }

    auto operator==(const IpV6Addr &lhs, const IpV6Addr &rhs) noexcept -> bool {
        return std::equal(std::begin(lhs.octets_), std::end(lhs.octets_), std::begin(rhs.octets_));
    }

    auto operator==(const IpV4Addr &lhs, const IpV6Addr &rhs) noexcept -> bool {
        return lhs.toIpV6Mapped() == rhs;
    }

    auto operator==(const IpAddr &lhs, const IpAddr &rhs) noexcept -> bool {
        return std::visit([](auto &&lhs, auto &&rhs) {
            return lhs == rhs;
        }, lhs.inner_, rhs.inner_);
    }

    auto operator<<(std::ostream &os, const IpV4Addr &addr) -> std::ostream & {
        os << static_cast<int>(addr.octets_[0]) << '.'
           << static_cast<int>(addr.octets_[1]) << '.'
           << static_cast<int>(addr.octets_[2]) << '.'
           << static_cast<int>(addr.octets_[3]);
        return os;
    }

    auto operator<<(std::ostream &os, const IpV6Addr &addr) -> std::ostream & {
        os << std::hex;
        for (size_t i = 0; i < sizeof(addr.octets_); ++i) {
            os << static_cast<int>(addr.octets_[i]);
            if (i != sizeof(addr.octets_) - 1) {
                os << ":";
            }
        }
        return os;
    }

    auto operator<<(std::ostream &os, const IpAddr &addr) -> std::ostream & {
        std::visit([&os](const auto &a) {
            os << a;
        }, addr.inner_);
        return os;
    }

} // namespace kkio::net