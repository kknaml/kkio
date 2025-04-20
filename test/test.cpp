
import std;
import kkio.coro.task;
import kkio.runtime;
import kkio.net.tcplistener;
import kkio.net.tcpstream;
#include <cstdint>
#include <cstring>

kkio::coro::Task<> aaa() {

    try {
        std::println("aaa");
        auto listener = kkio::net::TcpListener::bind("0.0.0.0", 24800);
        std::println("456789");
        auto stream = co_await listener.accept();
        std::println("Accepted {}", stream.innerFd());
        auto data = co_await stream.read(4096);
        std::println("Read {}\n{}", data.size(), (char *)data.data());
    } catch (std::exception &e) {
        std::println("exception {}", e.what());
    }
}

kkio::coro::Task<> bbb() {
    try {
        std::println("bbb");
        auto stream = co_await kkio::net::TcpStream::connect("www.gaotiexueyuan.com", 80);
        std::println("connect {}", stream.innerFd());
        const char *data = "GET / HTTP/1.1\r\n"
                           "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36 Edg/135.0.0.0\r\n"
                           "upgrade-insecure-requests: 1\r\n"
                           "host: www.gaotiexueyuan.com\r\n"
                           "sec-ch-ua: \"Microsoft Edge\";v=\"135\", \"Not-A.Brand\";v=\"8\", \"Chromium\";v=\"135\"\r\n"
                           "sec-ch-ua-mobile: ?0\r\n"
                           "sec-ch-ua-platform: \"Windows\"\r\n"
                           "\r\n";
        std::span<uint8_t> span = std::span{(uint8_t *)data, strlen(data)};
        auto r = co_await stream.write(span);
        std::println("Write {}", r);
        auto data2 = co_await stream.read(4096);
        std::println("Read {}\n{}", data2.size(), (char *)data2.data());
    } catch (std::exception &e) {
        std::println("exception {}", e.what());
    }
    co_return;
}

int main() {
    std::println("Hello World!");

    kkio::runtime::runBlocking(bbb());
    kkio::runtime::Runtime::GlobalRuntime->wait();
}
