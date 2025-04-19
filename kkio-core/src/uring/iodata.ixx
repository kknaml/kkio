module;

import std;

export module kkio.uring.iodata;

export namespace kkio::uring {

    enum class IOState {
        Idle,
        Running,
        Completed,
        Error,
        Cancelled
    };

    struct IOData {
        std::coroutine_handle<> handle_{nullptr};
        IOState state_{IOState::Idle};
        int result_{};
        void *buf_{nullptr};
    };

    inline auto operator==(const IOData &lhs, const IOData &rhs) noexcept -> bool {
        return lhs.handle_ == rhs.handle_ && lhs.result_ == rhs.result_;
    }

} // namespace kkio::uring
