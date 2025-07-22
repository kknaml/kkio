module;

#include <errno.h>
#include "openssl/ssl.h"
#include <openssl/bio.h>
#include <openssl/err.h>

import std;
import kkio.tls.boringssl.free;
import kkio.traits;
import kkio.error;
import kkio.coro.task;
import kkio.net.socket;
import kkio.uring.iodata;
import kkio.tls.config;

module kkio.tls.tls_stream;

using kkio::coro::Task;
using kkio::net::TcpStream;
using kkio::tls::boringssl::SSLPtr;
using kkio::tls::boringssl::SSL;
using kkio::tls::boringssl::SSL_CTX;
using kkio::tls::boringssl::SSL_CTXPtr;
using kkio::tls::boringssl::BIO;

namespace {
    using namespace kkio::tls;

    auto tls_handshake(
        TLSRole type,
        TcpStream &raw_stream,
        SSL *ssl,
        std::optional<const std::string_view> host = std::nullopt
    ) -> Task<> {
        std::println("handshake started");
        if (type == TLSRole::Client) {
            SSL_set_connect_state(ssl);
        } else {
            SSL_set_accept_state(ssl);
        }

        if (host.has_value()) {
            SSL_set_tlsext_host_name(ssl, host.value().data());
        }

        int ret{};
        auto *r_bio = SSL_get_rbio(ssl);
        auto *w_bio = SSL_get_wbio(ssl);
        std::println("SSL state: {}", SSL_state_string_long(ssl));
        std::println("SSL version: {}", SSL_get_version(ssl));
        std::println("SSL is_server: {}", SSL_is_server(ssl));

        while (true) {

            ret = SSL_do_handshake(ssl);
            std::println("do_handshake ret: {}", ret);
            if (ret == 1) {
                std::println("handsha");
                co_return;
            }

            {
                auto w_pending = BIO_pending(w_bio);
                std::println("wbio pending: {}", w_pending);
                if (w_pending > 0) {
                    std::println("Sending {} bytes from w_bio", w_pending);
                    std::array<uint8_t, 4096> buffer{};
                    while (BIO_pending(w_bio) > 0) {
                        auto len = BIO_read(w_bio, buffer.data(), buffer.size());
                        if (len <= 0) break;
                        const uint8_t *ptr = buffer.data();
                        auto remaining = len;
                        while (remaining > 0) {
                            auto sent = co_await raw_stream.send(std::span(ptr, remaining));
                            if (sent <= 0) {
                                throw std::runtime_error("Failed to send handshake data");
                            }
                            ptr += sent;
                            remaining -= sent;
                        }
                    }
                }
            }

            auto ssl_error = SSL_get_error(ssl, ret);
            std::println("ssl_error code: {}", ssl_error);
            switch (ssl_error) {
                case SSL_ERROR_WANT_READ: {
                    // SSL wants read, check if we have data in rbio
                    if (BIO_pending(r_bio) == 0) {
                        // no data in rbio, need read from socket
                        auto [data, len] = co_await raw_stream.recv(kkio::empty_span_buffer<uint8_t>(4096));
                        std::println("raw stream recv: {}", len);
                        if (len == 0) [[unlikely]] {
                            throw std::runtime_error(
                            std::format("Connection closed by peer during handshake (SSL_ERROR_WANT_READ, ret={}, msg = {})",
                                                            ret,  kkio::get_error_msg(ret))
                            );
                        }
                        if (len < 0) [[unlikely]]  {
                            throw std::runtime_error(
                            std::format("Handshake recv error (SSL_ERROR_WANT_READ, ret={}, msg = {})",
                                                            ret, kkio::get_error_msg(ret))
                            );
                        }

                        const uint8_t *ptr = data;
                        auto remaining = len;
                        while (remaining > 0) {
                            auto written = BIO_write(r_bio, ptr, remaining);
                            if (written <= 0) [[unlikely]] {
                                throw std::runtime_error(
                                std::format("BIO_write failed during handshake (SSL_ERROR_WANT_READ, ret={}, written={}, errno={}: {})",
                                    ret, written, errno, kkio::get_error_msg(errno))
                                );
                            }
                            ptr += written;
                            remaining -= written;
                        }
                    }
                    // if BIO has pending data, SSL will consume it in the next retry
                    break;
                }
                case SSL_ERROR_WANT_WRITE: {
                    auto pending = BIO_pending(w_bio);
                    if (pending > 0) {
                        std::array<uint8_t, 4096> buffer{};
                        while (pending > 0) {
                            auto to_read = std::min(pending, buffer.size());
                            auto len = BIO_read(w_bio, buffer.data(), to_read);
                            if (len <= 0) [[unlikely]] {
                                throw std::runtime_error(
                                    std::format("BIO_read failed during WANT_WRITE (ret={}, len={}, errno={}: {})",
                                    ret, len, errno, kkio::get_error_msg(errno))
                                );
                            }

                            const uint8_t *ptr = buffer.data();
                            auto remaining = len;
                            while (remaining > 0) {
                                auto sent = co_await raw_stream.send(std::span(ptr, remaining));
                                if (sent <= 0) [[unlikely]] {
                                    throw std::runtime_error(
                                        std::format("Failed to send handshake data (SSL_ERROR_WANT_WRITE, ret={}, sent={}, errno={}: {})",
                                        ret, sent, errno, kkio::get_error_msg(errno))
                                    );
                                }
                                ptr += sent;
                                remaining -= sent;
                            }
                            pending = BIO_pending(w_bio);
                        }
                    }
                    break;
                }
                case SSL_ERROR_SYSCALL: {
                    if (ERR_peek_error() == 0) {
                        if (ret == 0) {
                            throw std::runtime_error(
                                std::format("SSL handshake failed: connection closed (SSL_ERROR_SYSCALL, ret={}, errno={}: {})",
                                    ret, errno, kkio::get_error_msg(errno))
                            );
                        } else {
                            throw std::runtime_error(
                                std::format("SSL handshake failed: syscall error (SSL_ERROR_SYSCALL, ret={}, errno={}: {})",
                                    ret, errno, kkio::get_error_msg(errno))
                            );
                        }
                    }
                    [[fallthrough]];
                }
                case SSL_ERROR_SSL: {
                    auto err_code = ERR_get_error();
                    char err_buf[256];
                    ERR_error_string_n(err_code, err_buf, sizeof(err_buf));
                    throw std::runtime_error(
                        std::format("SSL handshake error (SSL_ERROR_SSL, ret={}, errno={}: {}, OpenSSL: {})",
                            ret, errno, kkio::get_error_msg(errno), err_buf)
                    );
                }
                case SSL_ERROR_ZERO_RETURN: {
                    throw std::runtime_error(
                        std::format("SSL connection closed cleanly during handshake (SSL_ERROR_ZERO_RETURN, ret={}, errno={}: {})",
                            ret, errno, kkio::get_error_msg(errno))
                    );
                }
                default: {
                    throw std::runtime_error(std::format("unexpected handshake error {}", ssl_error));
                }
            }
        }
        co_return;
    }

    auto create_client_ssl_ctx() -> SSL_CTXPtr {
        auto ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("SSL_CTX_new failed");
        }

        SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
        SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
        return SSL_CTXPtr(ctx);
    }

    auto flush_w_bio(
        TcpStream &raw_stream,
        BIO *w_bio
    ) -> Task<> {
        std::array<uint8_t, 4096> buffer{};
        while (true) {
            auto pending = BIO_pending(w_bio);
            if (pending <= 0) break;

            auto read = BIO_read(w_bio, buffer.data(), buffer.size());
            if (read < 0) {
                throw std::runtime_error(std::format("BIO_read failed duing flush_w_bio: {}", read));
            }
            const uint8_t *ptr = buffer.data();
            auto remaining = read;
            while (remaining > 0) {
                auto sent = co_await raw_stream.send(std::span(ptr, remaining));
                if (sent <= 0) {
                    throw std::runtime_error(std::format("Failed to send TLS data in flush_w_bio {}", sent));
                }
                ptr += sent;
                remaining -= sent;
            }
        }
        co_return;
    }

    auto fill_r_bio(
        TcpStream &raw_stream,
        BIO *r_bio
    ) -> Task<> {
        if (BIO_pending(r_bio) > 0) {
            co_return;
        }

        auto [data, len] = co_await raw_stream.recv(kkio::empty_span_buffer<uint8_t>(4096));
        if (len <= 0) {
            throw std::runtime_error(std::format("Error in stream.recv in fill_r_bio, {}", len));
        }

        auto written = BIO_write(r_bio, data, len);
        if (written <= 0) {
            throw std::runtime_error(std::format("Error in BIO_write in fill_r_bio, {}", written));
        }
        co_return;
    }
}

namespace kkio::tls {
    auto TLSStream::send(std::span<const uint8_t> data) -> Task<long> {
        auto *ssl = this->ssl.get();
        auto *w_bio = SSL_get_wbio(ssl);
        auto total_written = 0L;

        while (total_written < data.size()) {
            auto written = SSL_write(ssl, data.data() + total_written, data.size() - total_written);
            if (written < 0) {
                co_return written;
            }
            total_written += written;

            co_await flush_w_bio(this->raw_stream, w_bio);
        }

        co_return total_written;
    }

    auto TLSStream::recv(std::span<uint8_t> data) -> Task<uring::BufferResult> {
        auto *ssl = this->ssl.get();
        auto total_read = 0;
        // First try to read decrtpted data
        while (true) {
            auto read = SSL_read(ssl, data.data(), data.size());
            if (read < 0) {
                auto ssl_error = SSL_get_error(ssl, read);
                switch (ssl_error) {
                    case SSL_ERROR_WANT_READ:
                        break;
                    case SSL_ERROR_WANT_WRITE:
                        break;
                    default: {
                        throw std::runtime_error(std::format("SSL_read error in TLSStream::recv, {}", ssl_error));
                    }
                }

            }
            if (read > 0) {
                co_return {data.data(), read};
            }

            co_await fill_r_bio(this->raw_stream, SSL_get_rbio(ssl));
        }
    }

    auto TLSStream::connect(TcpStream raw, const TLSClientConfig &config) -> Task<TLSStream> {
        // TODO
        auto ctx = create_client_ssl_ctx();
        SSL_CTX_set_info_callback(ctx.get(), [](const SSL* ssl, int where, int ret) {
            if (where & SSL_CB_LOOP) {
                fprintf(stderr, "SSL State: %s\n", SSL_state_string_long(ssl));
            }
            else if (where & SSL_CB_ALERT) {
                fprintf(stderr, "SSL Alert: %s\n", SSL_alert_type_string_long(ret));
            }
        });
        auto ssl = SSLPtr(SSL_new(&*ctx));
        if (!ssl) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("SSL_new failed");
        }
        auto r_bio = kkio::tls::boringssl::kk_new_bio();
        if (!r_bio) {
            throw std::runtime_error("new rbio failed");
        }
        auto w_bio = kkio::tls::boringssl::kk_new_bio();
        if (!w_bio) {
            throw std::runtime_error("new wbio failed");
        }
        SSL_set_bio(ssl.get(), r_bio.release(), w_bio.release());
        co_await tls_handshake(TLSRole::Client, raw, ssl.get(), config.server_name);
        co_return TLSStream(std::move(ssl), std::move(raw), std::move(ctx));
    }
}
