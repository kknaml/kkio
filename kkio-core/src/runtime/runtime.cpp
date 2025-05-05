//module kkio.runtime;

import std;
import kkio.coro.task;
import kkio.runtime;

using kkio::coro::Task;

namespace kkio::runtime {


    Runtime::Runtime(int concurrency) : latch_(concurrency) {
    }

    auto Runtime::blockOn(Task<> task) -> void {
        auto context = task.context();
        auto oldRuntime = context->get<Runtime*>();
        if (oldRuntime == nullptr) [[likely]] {
            auto element = std::make_shared<RuntimeElement>(this);
            task.setContext(element);
            // todo post handle to worker
            latch_.wait();
            return;
        }
        throw std::runtime_error("Not implement yet");
    }


}