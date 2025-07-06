module;

import std;
import kkio.traits;

export module kkio.coro.cancellation;

export namespace kkio::coro {

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
    };

    class CancellationException final : public std::runtime_error {
    public:
        CancellationException();
        explicit CancellationException(std::string_view what);
    };
}
