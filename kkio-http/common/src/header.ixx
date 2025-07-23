module;

import std;
import kkio.traits;
import kkio.util.string;

export module kkio.http.header;

export namespace kkio::http {

    class HttpHeader {
    private:
        std::string key;
        std::string value;

    public:
        HttpHeader(std::string key, std::string value) : key(std::move(key)), value(std::move(value)) {}
        HttpHeader(std::string_view key, std::string_view value) : key(key), value(value) {}
        HttpHeader(HttpHeader &&other) noexcept : key(std::move(other.key)), value(std::move(other.value)) {}
        HttpHeader(const HttpHeader &other) : key(other.key), value(other.value) {}
        HttpHeader &operator=(HttpHeader &&other) noexcept {
            if (this != &other) {
                key = std::move(other.key);
                value = std::move(other.value);
            }
            return *this;
        }
        HttpHeader &operator=(const HttpHeader &other) {
            if (this != &other) {
                key = other.key;
                value = other.value;
            }
            return *this;
        }

        auto get_key() const -> std::string_view { return key; }
        auto get_value() const -> std::string_view { return value; }

        auto to_string() const -> std::string {
            return std::format("HttpHeader(key={}, value={})", key, value);
        }
    };

    constexpr auto operator==(const HttpHeader &lhs, const HttpHeader &rhs) noexcept -> bool {
        return util::case_ignore::equal(lhs.get_key(), rhs.get_key()) &&
            lhs.get_value() == rhs.get_value();
    }

    class HttpHeaderList final : NonCopy {
    private:
        std::vector<HttpHeader> headers;

    public:
        HttpHeaderList() : headers{} {}
        explicit HttpHeaderList(std::vector<HttpHeader> other) noexcept : headers(std::move(other)) {}
        HttpHeaderList(HttpHeaderList &&other) noexcept : headers(std::move(other.headers)) {}
        HttpHeaderList &operator=(HttpHeaderList &&other) noexcept {
            if (this != &other) {
                headers = std::move(other.headers);
            }
            return *this;
        }

        ~HttpHeaderList() = default;

        auto copy() const -> HttpHeaderList {
            return HttpHeaderList(this->headers);
        }

        auto add(HttpHeader header) -> void {
            headers.emplace_back(std::move(header));
        }

        auto add(std::string key, std::string value) -> void {
            headers.emplace_back(
                std::move(key),
                std::move(value)
            );
        }

        auto add(std::string_view key, std::string_view value) -> void {
            headers.emplace_back(
                std::string(key),
                std::string(value)
            );
        }


        auto find(std::string_view key) const noexcept {
            return headers
                | std::views::filter([&key](const HttpHeader &header) {
                    return util::case_ignore::equal(header.get_key(), key);
                });
        }

        auto get(std::string_view key) const -> std::optional<std::string_view> {
            auto matches = find(key);
            auto it = matches.begin();
            if (it != matches.end()) {
                return it->get_value();
            }
            return std::nullopt;
        }

        auto get_all(std::string_view key) const -> std::vector<std::string_view> {
            auto matches = find(key);
            return matches
                | std::views::transform([](const auto &h) {
                    return h.get_value();
                })
                | std::ranges::to<std::vector>();

        }

        auto begin(this auto &&self) {
            return self.headers.begin();
        }

        auto end(this auto &&self) {
            return self.headers.end();
        }

        auto to_string() const -> std::string {
            std::string result{};
            result += "HttpHeaderList:\n";
            for (const auto &header : *this) {
                result += std::format("{}: {}\n", header.get_key(), header.get_value());
            }
            return result;
        }

        auto operator==(const HttpHeaderList &rhs) const -> bool {
            return this->headers == rhs.headers;
        }
    };

    inline namespace keys {
        constexpr std::string_view ACCEPT          = "Accept";
        constexpr std::string_view ACCEPT_CHARSET  = "Accept-Charset";
        constexpr std::string_view ACCEPT_ENCODING = "Accept-Encoding";
        constexpr std::string_view ACCEPT_LANGUAGE = "Accept-Language";
        constexpr std::string_view AUTHORIZATION  = "Authorization";
        constexpr std::string_view CACHE_CONTROL   = "Cache-Control";
        constexpr std::string_view CONNECTION      = "Connection";
        constexpr std::string_view CONTENT_TYPE    = "Content-Type";
        constexpr std::string_view CONTENT_LENGTH  = "Content-Length";
        constexpr std::string_view CONTENT_ENCODING = "Content-Encoding";
        constexpr std::string_view COOKIE         = "Cookie";
        constexpr std::string_view DATE           = "Date";
        constexpr std::string_view EXPECT         = "Expect";
        constexpr std::string_view FROM           = "From";
        constexpr std::string_view HOST           = "Host";
        constexpr std::string_view IF_MATCH       = "If-Match";
        constexpr std::string_view IF_MODIFIED_SINCE = "If-Modified-Since";
        constexpr std::string_view IF_NONE_MATCH  = "If-None-Match";
        constexpr std::string_view IF_RANGE       = "If-Range";
        constexpr std::string_view IF_UNMODIFIED_SINCE = "If-Unmodified-Since";
        constexpr std::string_view LAST_MODIFIED  = "Last-Modified";
        constexpr std::string_view LOCATION       = "Location";
        constexpr std::string_view PRAGMA         = "Pragma";
        constexpr std::string_view PROXY_AUTHENTICATE = "Proxy-Authenticate";
        constexpr std::string_view RANGE          = "Range";
        constexpr std::string_view REFERER        = "Referer";
        constexpr std::string_view SERVER         = "Server";
        constexpr std::string_view SET_COOKIE     = "Set-Cookie";
        constexpr std::string_view TE             = "TE";
        constexpr std::string_view TRANSFER_ENCODING = "Transfer-Encoding";
        constexpr std::string_view UPGRADE        = "Upgrade";
        constexpr std::string_view USER_AGENT     = "User-Agent";
        constexpr std::string_view VARY           = "Vary";
        constexpr std::string_view VIA            = "Via";
        constexpr std::string_view WWW_AUTHENTICATE = "WWW-Authenticate";

    }

    inline namespace values {
        // MIME
        constexpr std::string_view JSON            = "application/json";
        constexpr std::string_view XML             = "application/xml";
        constexpr std::string_view OCTET_STREAM    = "application/octet-stream";
        constexpr std::string_view FORM_URLENCODED = "application/x-www-form-urlencoded";
        constexpr std::string_view TEXT_PLAIN      = "text/plain";
        constexpr std::string_view TEXT_HTML       = "text/html";
        constexpr std::string_view TEXT_CSS        = "text/css";
        constexpr std::string_view TEXT_JAVASCRIPT = "text/javascript";
        constexpr std::string_view IMAGE_PNG       = "image/png";
        constexpr std::string_view IMAGE_JPEG      = "image/jpeg";
        constexpr std::string_view IMAGE_GIF       = "image/gif";
        constexpr std::string_view IMAGE_SVG       = "image/svg+xml";

        constexpr std::string_view CHARSET_UTF8    = "charset=utf-8";

        constexpr std::string_view GZIP            = "gzip";
        constexpr std::string_view DEFLATE         = "deflate";
        constexpr std::string_view BR              = "br";

        constexpr std::string_view BEARER          = "Bearer";
        constexpr std::string_view BASIC           = "Basic";

        constexpr std::string_view NO_CACHE        = "no-cache";
        constexpr std::string_view NO_STORE        = "no-store";
        constexpr std::string_view MAX_AGE         = "max-age=";
        constexpr std::string_view MUST_REVALIDATE = "must-revalidate";

        constexpr std::string_view KEEP_ALIVE      = "keep-alive";
        constexpr std::string_view CLOSE           = "close";
    }
}
