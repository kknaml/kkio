module;

import std;
import kkio.traits;
import kkio.runtime.worker;

export module kkio.runtime.worker_pool;

export namespace kkio::runtime {

    class WorkerPool final : NonCopy {
    private:
        uint32_t parallel;
        std::vector<std::unique_ptr<RingWorker>> workers{};
        uint32_t next_id = 0;
    public:
        WorkerPool(
            uint32_t parallel,
            uint32_t ring_entries,
            uint32_t ring_flag,
            uint32_t bufs_in_ring_group,
            std::latch &latch
        );

        auto add_handle(std::coroutine_handle<> handle) noexcept -> void;

        auto shutdown() noexcept -> void;

    private:
        auto next_worker() noexcept -> RingWorker &;
    };
}
