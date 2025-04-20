module;

#include <liburing.h>
#include <iostream>

export module kkio.uring.awaiters:read;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Read : BaseUringAwaiter {

        Read(int fd, void *buf, uint32_t len, size_t offset)
        : BaseUringAwaiter(io_uring_prep_read, fd, buf, len, offset) {
            std::println("Read awaiter created with fd: {}, buf: {}, len: {}, offset: {}", fd, buf, len, offset);
        }

        auto await_resume() const noexcept -> int {
            std::println("Read awaiter resumed with result: {}", this->data.result_);
            return this->data.result_;
        }
    };

}
