module;

import std;
import kkio.traits;
import kkio.coro.awaiter_traits;
import kkio.uring.iodata;
import kkio.uring.awaiter.all;

export module kkio.net.socket;

using kkio::coro::Awaitable;
using kkio::uring::BufferResult;

export namespace kkio::net {

    namespace detail {
        class BaseSocket : NonCopy {
        protected:
            int fd;

            explicit BaseSocket(int fd) noexcept : fd(fd) {}
            BaseSocket(BaseSocket &&other) noexcept : fd(std::exchange(other.fd, -1)) {}
        public:

            auto get_fd() -> int { return fd; }
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
    };

}
