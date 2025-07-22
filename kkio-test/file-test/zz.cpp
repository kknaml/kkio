
#include <cstdio>

#include <openssl/ssl.h>
#include <openssl/err.h>

import std;
import kkio.coro.task;
import kkio.traits;
import kkio.coro.awaiter_traits;
import kkio.runtime.runtime;
import kkio.uring.awaiter.all;
import kkio.coro.cancellation;
import kkio.net.socket;
import kkio.net.sock_addr;
import kkio.tls.config;
import kkio.tls.tls_stream;

using kkio::coro::Task;
using namespace std::chrono_literals;
using namespace kkio::uring;
using namespace kkio::net;

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
    auto host= std::string("bing.com");
    auto addr = SocketAddress::from_string(host, 443);
    std::println("addr is {}", *addr);
    auto raw_stream = co_await kkio::net::TcpStream::connect(*addr);
    std::println("tcp stream connected, fd: {}", raw_stream.get_fd());
    auto config = kkio::tls::TLSClientConfig{};
    config.server_name = host;
    auto tls_stream = co_await kkio::tls::TLSStream::connect(std::move(raw_stream), config);
    std::println("TLSStream connected");
    std::string msg = "GET / HTTP/1.1\r\n"
                      "Host: bing.com\r\n"
                      "Connection: Close\r\n\r\n";
    auto msg_span = std::span<uint8_t>{(uint8_t *) msg.c_str(), msg.size()};
    auto sent = co_await tls_stream.send(msg_span);
    std::println("tls stream sent: {}", sent);
    auto buf = std::array<uint8_t, 4096>{};
    auto read_len = co_await tls_stream.recv(buf);
    // std::println("tls strean read: {}", read_len);
    // auto res = std::string_view((char *)buf.data, buf.size());
    std::println("res is {}", (char *)buf.data());
    co_await foo();
    co_return 123;
}


int main(int argc, char *argv[]) {

    setbuf(stdout, nullptr);
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
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
