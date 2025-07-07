export module kkio.coro.awaiter_traits;

import std;
import kkio.traits;
import kkio.uring.iodata;

export namespace kkio::coro {



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

