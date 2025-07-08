
module;

import std;
import kkio.traits;
import kkio.coro.cancellation;

export module kkio.uring.iodata;

using kkio::coro::CancellationToken;

export namespace kkio::uring {

    struct IOData {
        int io_result{};
        std::coroutine_handle<> io_handle{nullptr};
        union {
            uint8_t *buffer{nullptr};
            void *data;
            CancellationToken *cancel_token;
        };
    };

    struct BufferResult {
        uint8_t *data{nullptr};
        int size{};
    };
}
