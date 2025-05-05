export module kkio.runtime;

import std;
import kkio.traits;
import kkio.coro.task;
import kkio.runtime.worker;
import kkio.coro.context;

using kkio::coro::Task;
using kkio::coro::Element;

export namespace kkio::runtime {

    class Runtime;

    using RuntimeElement = Element<Runtime *>;

    class Runtime : NonCopy {
    private:
        std::latch latch_;

    public:
        explicit Runtime(int concurrency = 1);

        auto blockOn(Task<> task) -> void;
    };

} // namespace kkio::runtime
