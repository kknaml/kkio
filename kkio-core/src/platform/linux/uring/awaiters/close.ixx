module;

#include <liburing.h>

export module kkio.uring.awaiters:close;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Close : BaseUringAwaiter {
        Close(int fd)
            : BaseUringAwaiter{io_uring_prep_close, fd} {}

        auto await_resume() const noexcept -> int {
            return this->data.result_;
        }
    };

} 