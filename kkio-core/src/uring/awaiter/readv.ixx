module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:readv;

export namespace kkio::uring {

    struct ReadV final : BaseUringOP<ReadV> {

        ReadV(int fd, iovec *vecs, uint32_t nr_vecs, uint64_t offset)
        : BaseUringOP(io_uring_prep_readv, fd, vecs, nr_vecs, offset) {}

        auto await_resume() const -> int {
            return this->io_data.io_result;
        }
    };
}
