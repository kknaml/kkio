module;

#include <liburing.h>

export module kkio.uring.awaiters:readv;

import kkio.uring.base_awaiter;

export namespace kkio::uring {

    struct Readv : BaseUringAwaiter {
        Readv(int fd, const iovec *iov, int iovcnt, off_t offset)
            : BaseUringAwaiter{io_uring_prep_readv, fd, iov, iovcnt, offset} {}

        auto await_resume() const noexcept -> ssize_t {
            return this->data.result_;
        }
    };

} 