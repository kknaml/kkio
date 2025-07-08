module;

import std;
import kkio.traits;

module kkio.runtime.runtime;

namespace kkio::runtime {
    Runtime::Runtime(
        uint32_t parallel,
        uint32_t ring_entries,
        uint32_t ring_flag,
        uint32_t bufs_in_ring_group,
        std::latch &&latch
    ) : worker_pool(parallel, ring_entries, ring_flag, bufs_in_ring_group, latch) {
        latch.wait();
    }

    Runtime::Runtime(
        uint32_t parallel,
        uint32_t ring_entries,
        uint32_t ring_flag,
        uint32_t bufs_in_ring_group
     )
    : Runtime(parallel, ring_entries, ring_flag, bufs_in_ring_group, std::latch(parallel)) {
    }

    Runtime::~Runtime() {

    }
}
