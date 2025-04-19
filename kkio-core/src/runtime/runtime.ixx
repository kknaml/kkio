export module kkio.runtime;

import std;
import kkio.traits;
import kkio.coro.task;
import kkio.runtime.worker;

export namespace kkio::runtime {

    class Runtime : NonCopy {
    private:
        std::unique_ptr<WorkerManager> workerManager_;
    public:
        explicit Runtime(
            unsigned long workerSize = 1
        ) : workerManager_(std::make_unique<WorkerManager>(workerSize)) {}

        auto spawn(std::coroutine_handle<> task) {
            this->workerManager_->addTask(task);
        }

        auto wait() -> void {
            this->workerManager_->wait();
        }

        static Runtime *GlobalRuntime;
    };

    template<typename F>
    requires std::is_invocable_r_v<F, coro::Task<>>
    auto runBlocking(F &&fn) {
        auto task = fn();
        runBlocking(std::move(task));
    }

    auto runBlocking(coro::Task<> task) {
        auto runtime = new Runtime{};
        Runtime::GlobalRuntime = runtime;
        auto handle = task.takeHandle();
        runtime->spawn(handle);
    }

    auto spawn(coro::Task<> task) {
#ifdef DEBUG
        if (Runtime::GlobalRuntime == nullptr) {
            std::println("runtime is null");
            std::abort();
        }
#endif

        auto handle = task.takeHandle();
        Runtime::GlobalRuntime->spawn(handle);
    }

} // namespace kkio::runtime
