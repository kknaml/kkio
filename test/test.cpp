
import std;
import kkio.coro.task;
import kkio.runtime;
import kkio.net.tcplistener;
import kkio.net.tcpstream;

kkio::coro::Task<> aaa() {

    std::println("aaa");
    auto listener = kkio::net::TcpListener::bind("0.0.0.0", 24800);
    std::println("456789");
    auto stream = co_await listener.accept();
    std::println("Accepted {}", stream.innerFd());
    auto data = co_await stream.read(4096);
    std::println("Read {}\n{}", data.size(), (char *)data.data());
    co_return;
}

int main() {
    std::println("Hello World!");

    kkio::runtime::runBlocking(aaa());
    kkio::runtime::Runtime::GlobalRuntime->wait();
}
