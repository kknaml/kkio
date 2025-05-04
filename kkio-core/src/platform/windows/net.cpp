module kkio.windows.net;

#include <winsock2.h>
#include <Windows.h>
#include <cstdio>

import std;
import kkio.coro.task;
import kkio.traits;
import kkio.net.sockaddr;

using kkio::coro::Task;
using kkio::net::SocketAddr;
using kkio::net::SocketAddrV4;
using kkio::net::SocketAddrV6;

namespace kkio::windows::net {

    auto newSocket(const SocketAddr &addr, int ty) -> SOCKET {
        auto family = std::visit(Overloads {
            [](const SocketAddrV4) {
                return AF_INET;
            },
            [](const SocketAddrV6) {
                return AF_INET6;
            }
        }, addr.inner_);
        auto socket = WSASocketW(family, ty, 0, nullptr, 0,
            WSA_FLAG_OVERLAPPED | WSA_FLAG_NO_HANDLE_INHERIT);
        if (socket != INVALID_SOCKET) {
            return socket;
        }

        auto error = WSAGetLastError();
        if (error != WSAEPROTOTYPE && error != WSAEINVAL) {
            auto msg = std::format("WSASocketW failed {}\n{}", error, std::stacktrace::current());
            std::println(stderr, "{}", msg);
            throw std::runtime_error(msg);
        }

        socket = WSASocketW(family, ty, 0, nullptr, 0,
           WSA_FLAG_OVERLAPPED);

        if (socket == INVALID_SOCKET) {
            error = WSAGetLastError();
            auto msg = std::format("WSASocketW failed {}\n{}", error, std::stacktrace::current());
            std::println(stderr, "{}", msg);
            throw std::runtime_error(msg);
        }

        return socket;
    }

    auto TcpStream::connect(const SocketAddr &addr) -> Task<TcpStream> {
        auto sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create socket");
        }
        auto socket = newSocket(addr, 0);

    }

} // namespace kkio::windows::net
