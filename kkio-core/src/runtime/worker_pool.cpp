module;

import std;

module kkio.runtime.worker_pool;

namespace kkio::runtime {

    WorkerPool::WorkerPool(
        uint32_t parallel,
        uint32_t ring_entries,
        uint32_t ring_flag,
        uint32_t bufs_in_ring_group
    ) : parallel(parallel) {
        for (auto i = 0; i < ring_entries; i++) {
            this->workers.push_back(RingWorker::create(ring_entries, ring_entries, bufs_in_ring_group));
        }
    }

    auto WorkerPool::next_worker() noexcept -> RingWorker & {
        auto id = (this->next_id++) % this->workers.size();
        return *this->workers[id];
    }
}
