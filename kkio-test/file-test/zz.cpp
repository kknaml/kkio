import std;
import kkio.coro.task;

using kkio::coro::Task;

Task<> foo() {

    co_return;
}

Task<int> bar() {

    co_return 0;
}


int main(int argc, char *argv[]) {

    std::println("123: {}", "456");
}
