
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
}
