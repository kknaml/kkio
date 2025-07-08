
module;

import std;
import kkio.traits;
import kkio.coro.task;
import kkio.coro.cancellation;
import kkio.runtime.worker_pool;

export module kkio.runtime.runtime;

using kkio::coro::Task;

export namespace kkio::runtime {


    class Runtime final : NonCopy {
    private:
        WorkerPool worker_pool;
        std::mutex mutex{};
        std::condition_variable condition{};

    private:
        Runtime(
           uint32_t parallel,
           uint32_t ring_entries,
           uint32_t ring_flag,
           uint32_t bufs_in_ring_group,
           std::latch &&latch
       );

    public:

        Runtime(
            uint32_t parallel = std::thread::hardware_concurrency(),
            uint32_t ring_entries = 32,
            uint32_t ring_flag = 0,
            uint32_t bufs_in_ring_group = 32
        );

        ~Runtime();

        template<typename T>
        auto block_on(Task<T> task) -> T {
            auto handle = task.get_handle();
            auto &promise = handle.promise();
            if (!promise.cancel_token) {
                promise.cancel_token = std::make_shared<coro::CancellationToken>();
            }
            worker_pool.add_handle(handle);
            std::unique_lock lock(mutex);
            handle.promise().invoke_on_completion([&] {
               condition.notify_one();
            });
            condition.wait(lock, [&] {
                return handle.promise().has_value();
            });
            return std::move(handle.promise()).get_value();
        }


        auto block_on(auto &&f) -> decltype(auto) {
            return block_on(f());
        }
    };
}
