module;

import std;
import kkio.traits;
import kkio.coro.awaiter_traits;
import kkio.coro.base;
import kkio.uring.iodata;
import kkio.uring.awaiter.all;
import kkio.net.sock_addr;
import kkio.net.net_util;

export module kkio.net.socket;

using kkio::coro::Awaitable;
using kkio::uring::BufferResult;

export namespace kkio::net {

    class TcpStream;

    namespace detail {
        class BaseSocket : NonCopy {
        protected:
            int fd;

            explicit BaseSocket(int fd) noexcept : fd(fd) {}
            BaseSocket(BaseSocket &&other) noexcept : fd(std::exchange(other.fd, -1)) {}
        public:

            auto get_fd() const noexcept -> int { return fd; }
        };

        struct TcpStreamAwaiter final : coro::PhantomAwaiter<> {
            uring::Connect inner;

            explicit TcpStreamAwaiter(uring::Connect inner) noexcept : inner(std::move(inner)) {}

            auto await_ready() const noexcept -> bool { return inner.await_ready(); }

            auto await_suspend(auto &&handle) {
                return inner.await_suspend(std::forward<decltype(handle)>(handle));
            }

            auto await_resume() const -> TcpStream;
        };
    }

    class TcpStream : detail::BaseSocket {

    public:
        using BaseSocket::BaseSocket;
        explicit TcpStream(int fd) noexcept;;

        TcpStream(TcpStream &&other) noexcept;

        auto operator=(TcpStream &&other) noexcept -> TcpStream &;

        ~TcpStream();

        auto send(std::span<const uint8_t *> data, int flags = 0) -> Awaitable<int> auto {
            return uring::Send(fd, data.data(), data.size(), flags);
        }

        auto recv(std::span<uint8_t *> data, int flags = 0) -> Awaitable<BufferResult> auto {
            return uring::Recv(fd, data.data(), data.size(), flags);
        }

        static auto connect(SocketAddress &addr) -> Awaitable<TcpStream> auto {
            auto fd = new_tcp_socket(addr);
            fd = check_socket_fd_throw(fd);
            return detail::TcpStreamAwaiter(uring::Connect(fd, addr.to_addr(), addr.length()));
        }
    };

    namespace detail {
        auto TcpStreamAwaiter::await_resume() const -> TcpStream {
            auto fd = inner.await_resume();
            return TcpStream(fd);
        }

    }

}
