module;

#include <botan/auto_rng.h>
#include <botan/certstor.h>
#include <botan/certstor_system.h>
#include <botan/tls.h>

import std;
import kkio.net.tcpstream;
import kkio.coro.task;
import kkio.traits;

export module kkio.tls.botan;

using kkio::net::TcpStream;
using kkio::coro::Task;

export namespace kkio::tls {

    class BotanTlsWrapper;
    class KKBotanCallbacks;

    namespace detail {

    } // namespace detail

    class MyPolicy final : public Botan::TLS::Policy {
    public:
        auto require_cert_revocation_info() const -> bool override {
            return false;
        }
    };
    class ClientCredentials final : public Botan::Credentials_Manager {
    private:
        Botan::System_Certificate_Store cert_store_{};
    public:
        auto trusted_certificate_authorities(const std::string &type, const std::string &context) -> std::vector<Botan::Certificate_Store *> override {
            return {&cert_store_};
        }

        std::vector<Botan::X509_Certificate> find_cert_chain(
         const std::vector<std::string> &cert_key_types,
         const std::vector<Botan::AlgorithmIdentifier> &cert_signature_schemes,
         const std::vector<Botan::X509_DN> &acceptable_CAs,
         const std::string &type,
         const std::string &context) override {
            return {};
         }


    //    auto private_key_for(
    //     const Botan::X509_Certificate &cert,
    //     const std::string &type,
    //     const std::string &context
    //    ) -> Botan::Private_Key * override {
    //     return nullptr;
    //    }

    };

    class KKBotanCallbacks final : public Botan::TLS::Callbacks { 
    private:
        BotanTlsWrapper *wrapper_;
        std::vector<uint8_t> send_buffer_{};
        std::vector<uint8_t> recv_buffer_{};
    public:
        explicit KKBotanCallbacks(BotanTlsWrapper *wrapper) : wrapper_(wrapper) { }



        auto tls_emit_data(std::span<const uint8_t> data) -> void override;

        auto tls_record_received(uint64_t seq_no, std::span<const uint8_t> data) -> void override;

        auto tls_alert(Botan::TLS::Alert alert) -> void override { }

        auto wantWrite() const noexcept -> bool;

        auto wantRead() const noexcept -> bool;

        auto getReadBuffer() noexcept -> std::span<uint8_t>;

        auto clearReadBuffer() noexcept -> void;

        auto getWriteBuffer() noexcept -> std::span<uint8_t>;

        auto clearWriteBuffer() noexcept -> void;
    };


    class BotanTlsWrapper : NonCopy {
    private:
        TcpStream raw_stream_;
        std::unique_ptr<Botan::TLS::Client> client_{nullptr};
        KKBotanCallbacks *callbacks_{nullptr};

    public:
        BotanTlsWrapper(TcpStream stream, std::string_view host, int port = 443) : raw_stream_(std::move(stream)) {
            // TODO
            auto callback = std::make_shared<KKBotanCallbacks>(this);
            auto rng = std::make_shared<Botan::AutoSeeded_RNG>();
            auto session_manager = std::make_shared<Botan::TLS::Session_Manager_In_Memory>(rng);
            auto creds = std::make_shared<ClientCredentials>();
            auto policy = std::make_shared<MyPolicy>();
            client_ = std::make_unique<Botan::TLS::Client>(callback, session_manager, creds, policy, rng,
            Botan::TLS::Server_Information(host, port), Botan::TLS::Protocol_Version::TLS_V12);
            callbacks_ = callback.get();;
         }
    
        auto handshake() -> Task<void>;

        auto write(std::span<const uint8_t> data) -> Task<int>;

        auto read(size_t size) -> Task<std::span<const uint8_t>>;
        
    };

    // implemention

    auto KKBotanCallbacks::tls_emit_data(std::span<const uint8_t> data) -> void {
           std::println("tls_emit_data: {}", data.size());
    
        if (data.empty()) [[unlikely]] {
            return;
        }
        
        if (send_buffer_.size() > std::numeric_limits<size_t>::max() - data.size()) [[unlikely]] {
            throw std::runtime_error("Buffer overflow would occur");
        }
        
        auto current_size = send_buffer_.size();
        if (data.size() + current_size > send_buffer_.capacity()) {
            send_buffer_.reserve(data.size() + current_size);
        }
        
        send_buffer_.resize(data.size() + current_size);
        std::copy(data.begin(), data.end(), send_buffer_.begin() + current_size);
    }

    auto KKBotanCallbacks::tls_record_received(uint64_t seq_no, std::span<const uint8_t> data) -> void {
        std::println("tls_record_received: {}", data.size());
        if (recv_buffer_.size() > std::numeric_limits<size_t>::max() - data.size()) [[unlikely]] {
            throw std::runtime_error("Buffer overflow would occur");
        }
        auto current_size = recv_buffer_.size();
        recv_buffer_.resize(data.size() + current_size);
        std::copy(data.begin(), data.end(), recv_buffer_.begin() + current_size);
    }

    auto KKBotanCallbacks::wantWrite() const noexcept -> bool {
        return send_buffer_.size() > 0;
    }

    auto KKBotanCallbacks::wantRead() const noexcept -> bool {
        return recv_buffer_.size() > 0;
    }

    auto KKBotanCallbacks::getWriteBuffer() noexcept -> std::span<uint8_t> {
        return send_buffer_;
    }

    auto KKBotanCallbacks::getReadBuffer() noexcept -> std::span<uint8_t> {
        return recv_buffer_;
    }

    auto KKBotanCallbacks::clearWriteBuffer() noexcept -> void {
        std::println("clearWriteBuffer");
        send_buffer_.clear();
    }

    auto KKBotanCallbacks::clearReadBuffer() noexcept -> void {
        std::println("clearReadBuffer");
        recv_buffer_.clear();
    }

    auto BotanTlsWrapper::handshake() -> Task<void> {
        try {
            while (!client_->is_active()) {
                if (callbacks_->wantWrite()) { 
                    auto buffer = callbacks_->getWriteBuffer();
                    co_await raw_stream_.writeFully(buffer);
                    callbacks_->clearWriteBuffer();
                    continue;
                }
                auto data = co_await raw_stream_.read(4096);
                client_->received_data(data);
            }
            auto protocol_version = client_->application_protocol();
            std::println("TLS handshake success, protocol version: {}", protocol_version);
        } catch (std::exception &e) {
            std::println("TLS handshake failed: {}", e.what());
            raw_stream_.close();
            throw;
        }
    }

    auto BotanTlsWrapper::write(std::span<const uint8_t> data) -> Task<int> {
        try {
            int total_sent = 0;
            client_->send(data);
            while (callbacks_->wantWrite()) {
                auto buffer = callbacks_->getWriteBuffer();
                total_sent += co_await raw_stream_.writeFully(buffer);
                callbacks_->clearWriteBuffer();
            }
            co_return total_sent;
        } catch (std::exception &e) {
            std::println("TLS write failed: {}", e.what());
            throw;
        }
    }

    auto BotanTlsWrapper::read(size_t size) -> Task<std::span<const uint8_t>> {
        try {
            callbacks_->clearReadBuffer();
            auto data = co_await raw_stream_.read(size);
            client_->received_data(data);
            co_return callbacks_->getReadBuffer();
        } catch (std::exception &e) {
            std::println("TLS read failed: {}", e.what());
            throw;
        }
    }
    

} // namespace kkio::tls
