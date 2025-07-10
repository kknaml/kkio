module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:send;

export namespace kkio::uring {

    struct Send final : BaseUringOP<Send> {

        Send(int fd, const void *buf, size_t len, int flags)
        : BaseUringOP(io_uring_prep_send, fd, buf, len, flags) {

        }

        auto await_resume() const noexcept -> int {
            return this->io_data.io_result;
        }

    };
}
