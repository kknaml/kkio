module;

import std;
import kkio.traits;

export module kkio.util.string;

export namespace kkio::util {

    template<typename T>
    concept StringLike = is_one_of<std::string, std::string_view>;

    constexpr auto ascii_to_lower(char c) noexcept -> char {
        return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
    }

    auto str_to_lower(std::string_view str) noexcept -> std::string {
        return std::transform(str.begin(), str.end(), str.begin(), ascii_to_lower);
    }

    namespace case_ignore {

        auto equal(std::string_view a, std::string_view b) noexcept -> bool {
            return a.size() == b.size() &&
                std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) noexcept -> bool {
                    return ascii_to_lower(a) == ascii_to_lower(b);
                });
        }

        struct equal_to {
            static auto operator()(std::string_view a, std::string_view b) noexcept -> bool {
                return equal(a, b);
            }
        };
    }
}