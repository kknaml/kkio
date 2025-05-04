module;

#include <cstdio>
#include <Windows.h>

import kkio.traits;

export module kkio.windows.afd;

export namespace kkio::windows::afd {

    struct Afd {
        std::FILE fd_;
    };

    struct AfdPollHandleInfo {
        HANDLE handle_;
        uint32_t events_;
        NTSTATUS status_;
    };

    struct AfdPollInfo {
        int64_t timeout_;
        uint32_t numberOfHandles_;
        uint32_t exclusive_;
        AfdPollHandleInfo handles_[1];
    };


    constexpr inline uint32_t POLL_RECEIVE = 1 << 0;
    constexpr inline uint32_t POLL_RECEIVE_EXPEDITED = 1 << 1;
    constexpr inline uint32_t POLL_SEND = 1 << 2;
    constexpr inline uint32_t POLL_DISCONNECT = 1 << 3;
    constexpr inline uint32_t POLL_ABORT = 1 << 4;
    constexpr inline uint32_t POLL_LOCAL_CLOSE = 1 << 5;
    constexpr inline uint32_t POLL_CONNECT = 1 << 6;
    constexpr inline uint32_t POLL_ACCEPT = 1 << 7;
    constexpr inline uint32_t POLL_CONNECT_FAIL = 1 << 8;

    constexpr inline uint32_t KNOWN_EVENTS = POLL_RECEIVE
        | POLL_RECEIVE_EXPEDITED
        | POLL_SEND
        | POLL_DISCONNECT
        | POLL_ABORT
        | POLL_LOCAL_CLOSE
        | POLL_CONNECT
        | POLL_ACCEPT
        | POLL_CONNECT_FAIL;

} // namespace kkio::windows::afd
