module;

import std;

export module kkio.coro.base;

export namespace kkio::coro {

    template<typename T = void>
    struct PhantomAwaiter {

        static auto await_transform(T &&) -> decltype(auto) {
            static_assert(false, "PhantomAwaiter for T not impl");
        }
    };

    template<>
    struct PhantomAwaiter<void> {

    };


    template<typename T>
    constexpr auto get_awaiter(T &&t) -> decltype(auto) {
        if constexpr (std::derived_from<std::remove_reference_t<T>, PhantomAwaiter<>>) {
            return std::forward<T>(t);
        } else {
            return PhantomAwaiter<std::remove_reference_t<T>>::await_transform(std::forward<T>(t));
        }
    }

}
