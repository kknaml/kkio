
module;

#include <Windows.h>

import std;
import kkio.windows.iocp;
import kkio.traits;
import kkio.windows.afd;

export module kkio.windows.selector;


export namespace kkio::windows {

    enum class SockPollStatus {
        Idle,
        Pending,
        Cancelling
    };

    struct IO_STATUS_BLOCK {
        struct IO_STATUS_BLOCK0 {
            NTSTATUS Status_{};
            void *Pointer_{};
        };
        IO_STATUS_BLOCK0 Anonymous_{};
        uint64_t Information_{};
    };

    struct SockState {
        IO_STATUS_BLOCK iosb_;
        afd::AfdPollHandleInfo pollInfo_;
        std::shared_ptr<afd::Afd> afd_;
        uint64_t baseSocket;
        uint32_t userEvents_;
        uint32_t pendingEvents_;
        uint64_t userData_;
        SockPollStatus pollStatus_;
        bool deletePending_;
        std::optional<int32_t> error_;

    };

    class Selector {
        std::shared_ptr<iocp::CompletionPort> cp_;

    };

} // namespace kkio::windows
