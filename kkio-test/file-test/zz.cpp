
#include <cstdio>

import std;
import kkio.coro.task;
import kkio.traits;
import kkio.coro.awaiter_traits;
import kkio.runtime.runtime;
import kkio.uring.awaiter.all;

using kkio::coro::Task;
using namespace std::chrono_literals;

Task<> foo() {
    co_await 1s;
    co_await 10ms;
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

    std::println("pre block");
    auto result = runtime.block_on(bar);
    std::println("post block");

    std::println("hello: {}", result);
}
