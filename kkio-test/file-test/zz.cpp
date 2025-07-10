
#include <cstdio>

import std;
import kkio.coro.task;
import kkio.traits;
import kkio.coro.awaiter_traits;
import kkio.runtime.runtime;
import kkio.uring.awaiter.all;
import kkio.coro.cancellation;

using kkio::coro::Task;
using namespace std::chrono_literals;
using namespace kkio::uring;

Task<> foo() {
    co_await 10ms;
    co_await yield();
    // throw kkio::coro::CancellationException("WTF");
    // throw std::runtime_error("QAQ");
    auto *ctx = co_await kkio::coro::current_io_context();
    auto sub = kkio::runtime::launch(ctx, [] -> Task<int> {
        co_await 1000ms;
        co_return 1145;
    });
    co_await 10ms;
    std::println("sub is {}", co_await sub);
    co_await kkio::uring::delay(1);
    co_return;
}

Task<int> bar() {
    std::println("bar!!!");
    co_await foo();
    co_return 123;
}


int main(int argc, char *argv[]) {

    setbuf(stdout, nullptr);
    // static_assert(false, kkio::type_name<int>().data());

    auto runtime = kkio::runtime::Runtime(1);

    try {
        std::println("pre block");
        auto result = runtime.block_on(bar);
        std::println("post block");

        std::println("hello: {}", result);
    } catch (std::exception &e) {
        std::println("bar error: {}", e.what());
    }
}
