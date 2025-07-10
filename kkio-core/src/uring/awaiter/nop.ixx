module;

#include <liburing.h>

import kkio.uring.awaiter.base;
import kkio.uring.iodata;
import kkio.traits;

export module kkio.uring.awaiter.all:nop;

export namespace kkio::uring {

    struct Nop final : BaseUringOP<Nop> {

        Nop() : BaseUringOP(io_uring_prep_nop) {

        }

        constexpr auto await_resume() const -> void {

        }
    };

    auto yield() noexcept -> Nop {
        return {};
    }
}
