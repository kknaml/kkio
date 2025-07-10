export module kkio.coro.awaiter_traits;

import std;
import kkio.traits;
import kkio.uring.iodata;

export namespace kkio::coro {


    template<typename T>
    struct AwaitTrait {

        template<typename Promise>
        static auto extract_promise_type(std::coroutine_handle<Promise> handle) -> Promise {
            return handle.promise();
        }

        using suspend_arg_type = std::tuple_element_t<0, typename kkio::method_params_t<&T::await_suspend>>;
    };

    template<typename T, typename R, typename Trait = AwaitTrait<T>>
    concept Awaitable = requires(T &t) {
        { t.await_ready() } -> std::convertible_to<bool>;
        // requires std::same_as<decltype(t.await_suspend(std::declval<typename Trait::suspend_arg_type>())), void> ||
        //          std::same_as<decltype(t.await_suspend(std::declval<typename Trait::suspend_arg_type>())), bool> ||
        //          std::convertible_to<decltype(t.await_suspend(std::declval<typename Trait::suspend_arg_type>())), std::coroutine_handle<>>;
        { t.await_resume() } -> std::convertible_to<R>;
    };

    template<typename T>
    concept ReadAwaiter = requires(T &t) {
        { t.await_resume() };
        { t.await_suspend(std::declval<std::coroutine_handle<>>())};
        { t.await_resume() } -> std::convertible_to<uring::BufferResult>;
    };

    template<typename T>
    struct AsyncRead {

        static auto async_read(T &t, std::span<uint8_t> buf) -> ReadAwaiter auto {
            static_assert(false, "Type T does not implement trait AsyncRead");
        }
    };

    template<typename T>
    auto async_read(T &t, std::span<uint8_t> buf) {
        return AsyncRead<T>::async_read(t, buf);
    }
}

