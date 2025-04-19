module;

#include <cstdint>
#include <liburing.h>

import kkio.coro_base;
import kkio.uring.awaiters;
import std;

export module kkio.net.tcpstream;

export namespace kkio::net {

    namespace detail {

        struct ReadAwaiter : uring::Read {

            explicit ReadAwaiter(int fd, uint32_t size): uring::Read(fd, nullptr, size, 0) {
            }

            auto await_resume() -> std::span<uint8_t> {
                return std::span<uint8_t>{static_cast<uint8_t *>(data.buf_), static_cast<uint64_t>(data.result_)};
            }

            auto setFlagSub() -> void {
                std::println("setFlagSub()");
                this->sqe_->flags |= IOSQE_BUFFER_SELECT;
            }
        };
    }

    class TcpStream {
    private:
        int fd_;

    public:
        explicit TcpStream(int fd) : fd_(fd) {}

        auto innerFd() const noexcept -> int { return fd_; }

        auto read(uint32_t size) const -> coro::Awaitable<std::span<uint8_t>> auto {
            return detail::ReadAwaiter{fd_, size};
        }
    };
} // namespace kkio::net
