module;


// import std;
// import kkio.traits;


#include <liburing.h>


export module kkio.uring.context;

import kkio.traits;
import std;

export namespace kkio::uring {


    class Ring : public NonCopy {
        io_uring ring_{};

    public:
        explicit Ring() = default;

        auto inner(this auto &&self) noexcept -> auto && {
            return &self.ring_;
        }
        auto init() noexcept {

        }

        auto getSqe() noexcept -> io_uring_sqe * {
            return io_uring_get_sqe(&this->ring_);
        }

        auto submit() noexcept -> int {
            return io_uring_submit(&this->ring_);
        }

        auto waitCqe(io_uring_cqe **cqe) noexcept -> int {
            return io_uring_wait_cqe(&this->ring_, cqe);
        }
    };

    auto getLocalRing() noexcept -> Ring *;
    auto setLocalRing(Ring *ring) noexcept -> void;

} // namespace kkio::uring
