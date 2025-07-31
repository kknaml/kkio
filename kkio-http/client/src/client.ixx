module;

import std;
import kkio.traits;
import kkio.http.header;

export module kkio.http.client;

export namespace kkio::http::client {

    class Config final : NonCopy {
        HttpHeaderList default_headers{};
        HttpHeaderList connect_headers{};
        bool certs_verification{true};
        bool tls_sni{true};
        std::optional<Duration> connect_timeout{std::nullopt};
        std::optional<Duration> idle_timeout{std::nullopt};



    };

    class HttpClient final : NonCopy {

    };
}