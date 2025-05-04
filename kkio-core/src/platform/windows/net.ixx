
module;

#include <Windows.h>

import std;
import kkio.coro.task;
import kkio.traits;
import kkio.net.sockaddr;

export module kkio.windows.net;

using kkio::coro::Task;

using kkio::net::SocketAddr;

export namespace kkio::windows::net {

    auto newSocket(const SocketAddr &addr, int ty) -> SOCKET;

    class TcpStream : NonCopy {
    private:
        SOCKET inner;
    public:
        explicit TcpStream(SOCKET inner) : inner(inner) {}

        static auto connect(const SocketAddr &addr) -> Task<TcpStream>;
    };

} // namespace kkio::windows::net
