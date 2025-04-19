export module kkio.coro_base;

import std;
import kkio.traits;

export namespace kkio::coro {

    class AwaiterMarker {};

    template<typename T>
    struct await_traits {
        using suspend_arg_type = std::tuple_element_t<0, typename kkio::method_params_t<&T::await_suspend>>;
    };

    template<typename T, typename R, typename Trait = await_traits<T>>
    concept Awaitable = requires(T &t) {
        { t.await_ready() } -> std::convertible_to<bool>;
        requires std::same_as<decltype(t.await_suspend(std::declval<typename Trait::suspend_arg_type>())), void> ||
                 std::same_as<decltype(t.await_suspend(std::declval<typename Trait::suspend_arg_type>())), bool> ||
                 std::same_as<decltype(t.await_suspend(std::declval<typename Trait::suspend_arg_type>())), std::coroutine_handle<>>;
        { t.await_resume() } -> std::convertible_to<R>;
    };

} // namespace kkp::coro
