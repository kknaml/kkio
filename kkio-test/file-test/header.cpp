
import std;
import kkio.http.header;

using namespace kkio::http;

namespace {

    constexpr auto assert_eq(auto &&lhs, auto &&rhs, std::string_view msg = "") -> void {
        if (lhs != rhs) {
            throw std::runtime_error(std::string(msg));
        }
    }

    constexpr auto assert_ne(auto &&lhs, auto &&rhs, std::string_view msg = "") -> void {
        if (lhs == rhs) {
            throw std::runtime_error(std::string(msg));
        }
    }

    auto test_header_eq() {
        auto h1 = HttpHeader("accept", JSON);
        auto h2 = HttpHeader("Accept", JSON);
        assert_eq(h1, h2);
        auto h3 = HttpHeader(SET_COOKIE, JSON);
        assert_ne(h3, h1);

        auto l1 = HttpHeaderList{};
        l1.add(h1);
        l1.add(h2);
        l1.add(h3);

        auto l2 = HttpHeaderList{};
        l2.add("accept", JSON);
        l2.add(h2);
        l2.add(h3);
        assert_eq(l1, l2);
        std::println("{}", l2);
    }

    auto test_all() -> void {
        test_header_eq();
        std::println("Test Done");
    }
}



int main() {
    test_all();
}