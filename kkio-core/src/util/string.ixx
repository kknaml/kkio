module;

import std;
import kkio.traits;

export module kkio.util.string;


export namespace kkio::util {

    template<typename T>
    concept StringLike = is_one_of<std::string, std::string_view>;

    auto string_like_to_string(StringLike auto &&str) -> std::string {
        if constexpr (std::is_same_v<std::remove_reference_t<decltype(str)>, std::string>) {
            return str;
        } else {
            return std::string(str);
        }
    }

    constexpr auto ascii_to_lower(char c) noexcept -> char {
        return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
    }

    auto str_to_lower(std::string_view str) -> std::string {
        std::string result(str);
        std::ranges::transform(result, result.begin(), ascii_to_lower);
        return result;
    }

    namespace case_ignore {

        constexpr auto equal(std::string_view a, std::string_view b) noexcept -> bool {
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
