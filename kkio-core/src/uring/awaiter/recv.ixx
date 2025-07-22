module;

#include <liburing.h>

import std;
import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:recv;

export namespace kkio::uring {

    struct Recv final : BaseUringOP<Recv> {


        Recv(int fd, void *buf, size_t len, int flags)
        : BaseUringOP(io_uring_prep_recv, fd, buf, len, flags){
        }

        auto on_suspend() noexcept -> void {
            if (this->sqe->addr == 0) {
                this->sqe->flags |= IOSQE_BUFFER_SELECT;
            }
        }

        auto await_resume() -> BufferResult {
            return this->get_buffer_result();
        }
    };
}
