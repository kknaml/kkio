module;

import std;
import kkio.traits;

module kkio.runtime.runtime;

namespace {
    using kkio::runtime::Runtime;

    Runtime *global_runtime{nullptr};
}

namespace kkio::runtime {
    Runtime::Runtime(
        uint32_t parallel,
        uint32_t ring_entries,
        uint32_t ring_flag,
        uint32_t bufs_in_ring_group,
        std::latch &&latch
    ) : worker_pool(parallel, ring_entries, ring_flag, bufs_in_ring_group, latch) {
        latch.wait();
        Runtime::set_current(this);
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
        Runtime::set_current(nullptr);
    }

    auto Runtime::set_current(Runtime *runtime) -> void {
        if (runtime != nullptr && global_runtime != nullptr) {
            throw std::runtime_error("Runtime already set");
        }
        global_runtime = runtime;
    }

    auto Runtime::get_current() -> Runtime * {
        return global_runtime;
    }
}
