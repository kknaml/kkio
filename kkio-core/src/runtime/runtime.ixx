
module;

import std;
import kkio.traits;
import kkio.coro.task;
import kkio.coro.base;
import kkio.coro.cancellation;
import kkio.runtime.worker_pool;

export module kkio.runtime.runtime;

using kkio::coro::Task;
using kkio::coro::JoinHandle;

export namespace kkio::runtime {

    class Runtime;

    // struct RuntimeAwaiter final : coro::PhantomAwaiter<> {
    //     Runtime *runtime{nullptr};
    //
    //     constexpr auto await_ready() const noexcept {
    //         return false;
    //     }
    //
    //     template<typename Promise>
    //     auto await_suspend(std::coroutine_handle<Promise> handle) noexcept -> bool {
    //
    //         return false;
    //     }
    //
    //     auto await_resume() -> Runtime * {
    //         return runtime;
    //     }
    // };

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
                return handle.promise().is_done();
            });
            if (promise.cancel_token->is_canceled()) {
                throw coro::CancellationException(std::format("Task was canceled: {}", promise.cancel_token->get_msg()));
            }
            return std::move(handle.promise()).get_value();
        }


        auto block_on(auto &&f) -> decltype(auto) {
            return block_on(f());
        }

        template<typename T>
        auto launch(coro::IOContext *ctx, Task<T> task) -> JoinHandle<T> {
            auto handle = task.take_handle();
            handle.promise().cancel_token = *ctx->get_cancel_token();
            worker_pool.add_handle(handle);
            return JoinHandle<T>(handle);
        }

        auto launch(coro::IOContext *ctx, auto &&f) -> decltype(auto) {
            return launch(ctx, f());
        }

        static auto set_current(Runtime *runtime) -> void;
        static auto get_current() -> Runtime *;
    };

    template<typename T>
    auto launch(coro::IOContext *ctx, Task<T> task) -> JoinHandle<T> {
        return Runtime::get_current()->launch(ctx, std::move(task));
    }

    auto launch(coro::IOContext *ctx, auto &&f) -> decltype(auto) {
        return launch(ctx, f());
    }
}
