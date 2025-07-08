module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:read;

export namespace kkio::uring {

    struct Read final : BaseUringOP<Read> {

        Read(int fd, void *buf, uint32_t nbytes, uint64_t offset)
        : BaseUringOP(io_uring_prep_read, fd, buf, nbytes, offset) {

        }

        auto await_resume() -> BufferResult {
            return this->get_buffer_result();
        }
    };
}
