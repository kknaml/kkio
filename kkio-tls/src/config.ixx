module;

import std;
import kkio.traits;
import kkio.tls.ciphers;

export module kkio.tls.config;

export namespace kkio::tls {

    enum class TLSRole {
        Client, Server
    };

    enum class TLSVersion {
        TLS_1_0,
        TLS_1_1,
        TLS_1_2,
        TLS_1_3,
    };

    enum class HandshakeResult {
        Complete,
        WantRead,
        WantWrite,
        Error
    };

    enum class IOResult {
        Success,
        WantRead,
        WantWrite,
        ConnectionClosed,
        Error
    };

    struct TLSBaseConfig {
        TLSVersion min_version = TLSVersion::TLS_1_1;
        TLSVersion max_version = TLSVersion::TLS_1_3;
        std::vector<Cipher> ciphers{};
        std::vector<std::string> alpn_protocols{};
    };

    struct TLSClientConfig final : public TLSBaseConfig {
        std::string server_name{};


        static auto create() -> std::unique_ptr<TLSClientConfig> {
            return std::make_unique<TLSClientConfig>();
        }
    };

    struct TLSServerConfig final : public TLSBaseConfig {

        static auto create() -> std::unique_ptr<TLSServerConfig> {
            return std::make_unique<TLSServerConfig>();
        }
    };
}