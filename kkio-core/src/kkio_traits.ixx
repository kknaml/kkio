
export module kkio.traits;

import std;

export import kkio.kkint;
export import kkio.error;

export namespace kkio {

    class NonCopy {
    public:
        constexpr NonCopy() = default;
        ~NonCopy() = default;
        NonCopy(const NonCopy &) = delete;
        NonCopy &operator=(const NonCopy &) = delete;
    };

    template<typename ...Ts>
    struct Overloads : Ts... {
        using Ts::operator()...;
    };

    template<typename T, typename R, typename ...Args>
    concept Function = std::is_invocable_r_v<R, T, Args...>;

    template<typename T, typename ...Args>
    concept Action = Function<T, void, Args...>;

    template<Action F>
    struct Defer final : NonCopy {
        F destructor_fn;
        constexpr Defer(F destructor_fn) : destructor_fn(destructor_fn) {}
        ~Defer() noexcept(noexcept(destructor_fn())) {
            destructor_fn();
        }
    };

    template <auto MethodPtr>
    constexpr auto get_method_params() {
        return []<typename T, typename R, typename... Args>(R(T::*)(Args...)) {
            return std::type_identity<std::tuple<Args...>>{};
        }(MethodPtr);
    }

    template <auto MethodPtr>
    using method_params_t = typename decltype(get_method_params<MethodPtr>())::type;

    template<typename T, typename ...Types>
    concept is_one_of = std::disjunction_v<std::is_same<T, Types>...>;

    struct  Default {
        template<typename T>
        requires std::default_initializable<T>
        constexpr operator T() const noexcept(noexcept(T{})) {
            return T{};
        }
    };

    constexpr inline Default default_value{};

    // TODO
    template<typename T>
    consteval std::string_view type_name() {
        constexpr std::string_view prefix = "[with T = ";
        constexpr std::string_view suffix = "]";
        constexpr std::string_view func = __PRETTY_FUNCTION__;
        constexpr auto start = func.find(prefix) + prefix.size();
        constexpr auto end = func.find(suffix, start);
        return func.substr(start, end - start);
    }

    auto TODO(std::string_view msg = "") {
        std::println("{} not impl", msg);
        std::abort();
    }

    template<typename T>
    constexpr auto empty_span_buffer(std::size_t size) noexcept -> std::span<T> {
        return std::span<T> {static_cast<T *>(nullptr), size};
    }

    template<typename T>
    concept HasToString = requires(const T &t) {
        { t.to_string() } -> std::convertible_to<std::string_view>;
    };

    using Duration = std::chrono::milliseconds;

}

template<kkio::HasToString T>
struct std::formatter<T> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const T &obj, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "{}", obj.to_string());
    }
};
