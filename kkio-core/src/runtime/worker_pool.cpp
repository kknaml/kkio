module;

import std;
import kkio.traits;
import kkio.runtime.worker;

module kkio.runtime.worker_pool;

namespace kkio::runtime {

    WorkerPool::WorkerPool(
        uint32_t parallel,
        uint32_t ring_entries,
        uint32_t ring_flag,
        uint32_t bufs_in_ring_group,
        std::latch &latch
    ) : parallel(parallel) {
        for (auto i = 0; i < parallel; i++) {
            this->workers.push_back(RingWorker::create(ring_entries, ring_flag, bufs_in_ring_group, latch));
        }
    }

    auto WorkerPool::add_handle(std::coroutine_handle<> handle) noexcept -> void {
        auto &worker = this->next_worker();
        worker.add_handle(handle);
        worker.notify();
    }

    auto WorkerPool::shutdown() noexcept -> void {
        for (auto &worker : this->workers) {
            worker->shutdown();
        }
    }

    auto WorkerPool::next_worker() noexcept -> RingWorker & {
        auto id = (this->next_id++) % this->workers.size();
        return *this->workers[id];
    }
}
