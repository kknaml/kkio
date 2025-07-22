module;

#include <openssl/ssl.h>
#include <openssl/bio.h>

import std;
import kkio.tls.boringssl.free;
import kkio.traits;
import kkio.coro.task;
import kkio.net.socket;
import kkio.uring.iodata;
import kkio.tls.config;

export module kkio.tls.tls_stream;

using kkio::coro::Task;
using kkio::net::TcpStream;
using kkio::tls::boringssl::SSLPtr;
using kkio::tls::boringssl::SSL;
using kkio::tls::boringssl::SSL_CTX;
using kkio::tls::boringssl::SSL_CTXPtr;



export namespace kkio::tls {

    class TLSStream final : public NonCopy {
        SSLPtr ssl;
        TcpStream raw_stream;
        SSL_CTXPtr ctx; // nullptr for optional

    public:
        TLSStream(SSLPtr ssl, TcpStream raw_stream, SSL_CTXPtr ctx = nullptr)
        : ssl(std::move(ssl)), raw_stream(std::move(raw_stream)), ctx(std::move(ctx)) {}

        TLSStream(TLSStream &&other) noexcept
        : ssl(std::move(other.ssl)), raw_stream(std::move(other.raw_stream)), ctx(std::move(other.ctx)) {}

        auto operator=(TLSStream &&other) noexcept -> TLSStream & {
            if (this != &other) {
                this->ssl = std::move(other.ssl);
                this->raw_stream = std::move(other.raw_stream);
                this->ctx = std::move(other.ctx);
            }
            return *this;
        }

        ~TLSStream() = default;

        auto get_fd() const noexcept -> int {
            return raw_stream.get_fd();
        }

        auto send(std::span<const uint8_t> data) -> Task<long>;

        auto recv(std::span<uint8_t> data) -> Task<uring::BufferResult>;

    private:

    public:
        static auto connect(TcpStream raw, const TLSClientConfig &config) -> Task<TLSStream>;
    };
}