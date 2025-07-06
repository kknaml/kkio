
module kkio.coro.cancellation;

import std;

namespace kkio::coro {

    CancellationToken::CancellationToken() = default;

    auto CancellationToken::is_canceled() const noexcept -> bool {
        return this->canceled;
    }

    auto CancellationToken::cancel(std::string msg) noexcept -> bool {
        std::vector<CancellationCallback> to_invoke{};
        {
            std::lock_guard lock{this->mtx};
            if (this->canceled) return false;
            this->cancel_msg = std::move(msg);
            this->canceled = true;
            to_invoke = std::move(this->callbacks);
        }
        for (auto &cb : to_invoke) {
            cb(*this);
        }
        return true;
    }

    auto CancellationToken::invoke_on_cancellation(CancellationCallback cb) noexcept -> void {
        std::lock_guard lock{this->mtx};
        if (this->canceled) {
            cb(*this);
        } else {
            this->callbacks.push_back(std::move(cb));
        }
    }

    CancellationException::CancellationException() : std::runtime_error("Operation was canceled") {}

    CancellationException::CancellationException(std::string_view what)
        : std::runtime_error(std::string(what)) {}
}
