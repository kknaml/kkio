
#include <cstdint>
#include <print>

import std;
// import kkio.util.utils;
// import kkio.coro.task;
// import kkio.tls.botan;
// import kkio.runtime;
// import kkio.net.tcpstream;
// import kkio.traits;
//
// using kkio::coro::Task;
//
// const char data[] = "GET / HTTP/1.1\r\nHost: baidu.com\r\n\r\n";
//
// Task<> test_tls() {
//     try {
//       auto stream = co_await kkio::net::TcpStream::connect("baidu.com", 443);
//       auto tls = kkio::tls::BotanTlsWrapper(std::move(stream), "baidu.com", 443);
//          co_await tls.handshake();
//          auto sent = co_await tls.write(std::span{(const uint8_t*)data, sizeof(data)});
//          std::println("sent: {}", sent);
//          auto received = co_await tls.read(4096);
//          std::println("received: {}", received.size());
//          std::println("received: {}", std::string_view{(const char*)received.data(), received.size()});
//
//     } catch (const std::exception &e) {
//       std::println("Error: {}", e.what());
//     }
// }
//

using namespace std::chrono_literals;

int main() {

    auto a = 1s;


    std::println("123");

   return 0;
}