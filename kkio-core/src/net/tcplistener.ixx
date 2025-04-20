module;

import kkio.coro_base;
import kkio.uring.awaiters;
import kkio.traits;
import kkio.net.netutils;
import kkio.net.tcpstream;
import std;

#include <netinet/in.h>

export module kkio.net.tcplistener;


export namespace kkio::net {

    namespace detail {
        struct AcceptAwaiter : uring::Accept {
            sockaddr_in addr_{};
            socklen_t addrlen_ = sizeof(addr_);
            explicit AcceptAwaiter(int fd, int flag = 0) : Accept(fd, reinterpret_cast<sockaddr *>(&addr_), &addrlen_, flag) {
            }

            [[nodiscard]]
            auto await_resume() const -> TcpStream {
                auto fd = Accept::await_resume();
                if (fd < 0) {
                    throw std::runtime_error(std::format("accept failed: {}", fd));
                }
                return TcpStream{fd};
            }
        };
    } // namespace detail

    class TcpListener : NonCopy {
    private:
    int fd_{-1};

    public:
        explicit TcpListener(int fd) noexcept : fd_(fd) {}

        TcpListener(TcpListener &&other) noexcept : fd_(std::exchange(other.fd_, -1)) {}

        TcpListener &operator=(TcpListener &&other) noexcept {
            if (this != &other) {
                this->fd_ = std::exchange(other.fd_, -1);
            }
            return *this;
        }

        [[nodiscard]]
        auto accept(int flag = 0) const -> coro::Awaitable<TcpStream> auto {
            return detail::AcceptAwaiter{fd_, flag};
        }

        ~TcpListener() noexcept {
            close();
        }

        auto close() noexcept -> int {
            int fd = std::exchange(fd_, -1);
            if (fd >= 0) {
                closeSocket(fd);
            }
        }

    public:

        static auto bind(std::string_view host, int port) -> TcpListener {
            auto fd = createServerSocket(host, port);
            return TcpListener{fd};
        }
    };
} // namespace kkio::net
