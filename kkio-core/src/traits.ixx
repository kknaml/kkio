
module;

import std;

export module kkio.traits;

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

} // namespace kkio
