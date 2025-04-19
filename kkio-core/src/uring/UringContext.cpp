module kkio.uring.context;

import std;
// import kkio.uring.context;

namespace kkio::uring {

    thread_local Ring *local_ring{nullptr};

    auto getLocalRing() noexcept -> Ring * {
        return local_ring;
    }
    auto setLocalRing(Ring *ring) noexcept -> void {
        local_ring = ring;
    }

} // namespace kkio::uring
