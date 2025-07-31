module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:writev;

export namespace kkio::uring {

    struct WriteV final : BaseUringOP<WriteV> {

        WriteV(int fd, const iovec *iovecs, uint32_t nr_vecs, uint64_t offset)
        : BaseUringOP(io_uring_prep_writev, fd, iovecs, nr_vecs, offset) {}

        auto await_resume() const -> int {
            return this->io_data.io_result;
        }
    };

}