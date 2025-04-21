module;

#include <liburing.h>

export module kkio.uring.awaiters:write;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Write : BaseUringAwaiter {

        Write(int fd, void *buf, uint32_t len, size_t offset)
        : BaseUringAwaiter(io_uring_prep_write, fd, buf, len, offset) {}


        auto await_resume() const noexcept -> int {
            return this->data.result_;
        }
    };

}
