module;

import std;
import kkio.traits;
import kkio.coro.base;

export module kkio.coro.cancellation;

export namespace kkio::coro {

    class CancellationToken;

    namespace detail {

        struct CancellationTokenAwaiter final : PhantomAwaiter<> {
            std::shared_ptr<CancellationToken> *token{nullptr};
            constexpr auto await_ready() const noexcept -> bool {
                return false;
            }

            template<typename Promise>
            auto await_suspend(std::coroutine_handle<Promise> handle) noexcept -> bool {
                this->token = &handle.promise().cancel_token;
                return true;
            }

            auto await_resume() const noexcept -> std::shared_ptr<CancellationToken> * {
                return this->token;
            }
        };
    }

    class CancellationToken final : NonCopy {
    public:
        using CancellationCallback = std::function<void(CancellationToken &)>;
    private:
        std::string cancel_msg{};
        bool canceled{false};
        mutable std::mutex mtx{};
        std::vector<CancellationCallback> callbacks{};
    public:

        explicit CancellationToken();

        [[nodiscard]]
        auto is_canceled() const noexcept -> bool;

        auto cancel(std::string msg) noexcept -> bool;

        auto invoke_on_cancellation(CancellationCallback cb) noexcept -> void;

        auto invoke_cancellation_cb() -> void;

        auto get_msg() const noexcept -> std::string_view;

        static auto current() -> detail::CancellationTokenAwaiter;
    };

    class CancellationException final : public std::runtime_error {
    public:
        CancellationException();
        explicit CancellationException(std::string_view what);
    };
}
