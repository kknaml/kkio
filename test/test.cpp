
import std;
import kkio.task;

kkio::coro::Task<int> aaa() {

    co_return 20;
}

int main() {

    std::println("Hello World!");
}