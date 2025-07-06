
module;

import std;
import kkio.traits;

export module kkio.uring.iodata;

export namespace kkio::uring {

    struct IOData {
        int io_result{};
        std::coroutine_handle<> io_handle{nullptr};
        uint8_t *buffer{nullptr};
    };

    struct BufferResult {
        uint8_t *data{nullptr};
        int size{};
    };
}
