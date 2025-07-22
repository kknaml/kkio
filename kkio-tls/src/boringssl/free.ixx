module;

#include <cstdio>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

import std;

export module kkio.tls.boringssl.free;

export namespace kkio::tls::boringssl {

    template<auto Fn>
    class OpenSSLFree {
    public:
        auto operator()(auto *obj) const noexcept(noexcept(Fn)) -> void {
            if (obj != nullptr) [[likely]] {
                if constexpr (std::is_same_v<decltype(Fn(obj)), int>) {
                    auto res = Fn(obj);
                    if (res != 1) {
                        std::println(stderr, "OpenSSL free failed. code: {}, fn: {}", res, __PRETTY_FUNCTION__);
#ifdef KKIO_DEBUG
                        std::abort();
#endif
                    }
                } else {
                    Fn(obj);
                }
            }
        }
    };

    template<typename T, auto FreeFn>
    requires std::is_invocable_v<decltype(FreeFn), T*>
    using OpenSSLPtrType = std::unique_ptr<T, OpenSSLFree<FreeFn>>;

    using ::SSL;
    using ::BIO;
    using ::X509;
    using ::SSL_CTX;

    using SSLPtr = OpenSSLPtrType<SSL, SSL_free>;
    using BIOPtr = OpenSSLPtrType<BIO, BIO_free>;
    using X509Ptr = OpenSSLPtrType<X509, X509_free>;
    using SSL_CTXPtr = OpenSSLPtrType<SSL_CTX, SSL_CTX_free>;

    auto kk_new_raw_bio() -> BIO * {
        return BIO_new(BIO_s_mem());
    }

    auto kk_new_bio() -> BIOPtr {
        return BIOPtr(kk_new_raw_bio());
    }
}