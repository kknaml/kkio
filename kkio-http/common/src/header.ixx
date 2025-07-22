module;

import std;
import kkio.traits;
import kkio.util.string;

export module kkio.http.common.header;

export namespace kkio::http::header {

    class HttpHeader {
    private:
        std::string key;
        std::string value;

    public:
        HttpHeader(std::string key, std::string value) : key(std::move(key)), value(std::move(value)) {}
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
    };

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
            headers.emplace_back(std::move(key), std::move(value));
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
                return *it;
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
    };
}