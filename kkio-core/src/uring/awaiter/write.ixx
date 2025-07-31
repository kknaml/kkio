module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:write;

export namespace kkio::uring {

    struct Write final : BaseUringOP<Write> {

        Write(int fd, const void *buf, uint32_t nbytes, uint64_t offset)
        : BaseUringOP(io_uring_prep_write, fd, buf, nbytes, offset) {

        }

        auto await_resume() const noexcept -> int {
            return this->io_data.io_result;
        }
    };
}
