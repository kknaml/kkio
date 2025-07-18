module;

import std;
import kkio.traits;

export module kkio.tls.ciphers;

export namespace kkio::tls {

    struct Cipher {
        uint16_t code;
        std::string_view name;

        constexpr Cipher(uint16_t code, std::string_view name) : code(code), name(name) {}

        explicit constexpr operator uint16_t() const noexcept {
            return code;
        }

        constexpr auto to_string() const noexcept -> std::string_view {
            return name;
        }
    };

#define DEF_CIPHER(code, name) \
    constexpr Cipher name = Cipher(code, #name);

    DEF_CIPHER(0x0005, TLS_RSA_WITH_RC4_128_SHA);
    DEF_CIPHER(0x000a, TLS_RSA_WITH_3DES_EDE_CBC_SHA);
    DEF_CIPHER(0x002f, TLS_RSA_WITH_AES_128_CBC_SHA);


    constexpr std::array ALL_CIPHERS = {
        TLS_RSA_WITH_RC4_128_SHA,
        TLS_RSA_WITH_3DES_EDE_CBC_SHA
    };
}