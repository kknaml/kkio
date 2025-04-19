export module kkio.traits;

import std;

export namespace kkio {

    class NonCopy {
    public:
        constexpr NonCopy() = default;
        NonCopy(const NonCopy &) = delete;
        NonCopy &operator=(const NonCopy &) = delete;
    };

    struct Unit final {};
    constexpr inline Unit unit{};

    template<typename T, typename R, typename... Args>
    concept Function = std::is_invocable_r_v<R, T, Args...>;

    template<typename T, typename ...Args>
    concept Action = Function<T, void, Args...>;

    template<typename ...Ts>
    struct Overloads : Ts... {
        using Ts::operator()...;
    };

    template<Action F>
    struct Defer : NonCopy {
        F destructor_fn_;
        constexpr explicit Defer(F &&fn) : destructor_fn_(fn) {}

        ~Defer() noexcept(noexcept(destructor_fn_())) {
            destructor_fn_();
        }
    };

    class Error {
    public:
        int code{};
        std::string message{};
    };

    template<typename T>
    using Result = std::expected<T, Error>;

    template<typename T, typename E = std::uint8_t>
    concept Buffer = requires(T t) {
        { t.data() } -> std::convertible_to<const std::remove_reference_t<T> *>;
        { t.size() } -> std::convertible_to<std::size_t>;
    };

    template<typename T, typename E = std::uint8_t>
    concept MutableBuffer = requires(T t) {
        { t.data() } -> std::convertible_to<std::remove_reference_t<T> *>;
        { t.size() } -> std::convertible_to<std::size_t>;
    };

    template <auto MethodPtr>
    constexpr auto get_method_params() {
        return []<typename T, typename R, typename... Args>(R(T::*)(Args...)) {
            return std::type_identity<std::tuple<Args...>>{};
        }(MethodPtr);
    }

    template <auto MethodPtr>
    using method_params_t = typename decltype(get_method_params<MethodPtr>())::type;

} // namespace kkio
