module;

#include <cstdint>
#include <liburing.h>
#include <__ranges/data.h>

import kkio.coro_base;
import kkio.uring.awaiters;
import kkio.uring.base_awaiter;
import std;

export module kkio.net.tcpstream;

export namespace kkio::net {

    namespace detail {

        struct ReadAwaiter : uring::Read {

            explicit ReadAwaiter(int fd, uint32_t size): uring::Read(fd, nullptr, size, 0) {
            }

            auto await_resume() -> std::span<uint8_t> {
                if (data.result_ < 0) {
                    throw std::runtime_error(std::format("read failed: {}", data.result_));
                }
                return std::span<uint8_t>{static_cast<uint8_t *>(data.buf_), static_cast<uint64_t>(data.result_)};
            }

            auto await_suspend(std::coroutine_handle<> handle) noexcept -> void {
                setFlagSub();
                uring::BaseUringAwaiter::await_suspend(handle);
            }

            auto setFlagSub() -> void {
                std::println("Setting flags: IOSQE_BUFFER_SELECT");
                this->sqe_->flags |= IOSQE_BUFFER_SELECT;
                std::println("Setting buf_group to 1");
                this->sqe_->buf_group = 1;
                std::println("SQE flags after setup: {}", this->sqe_->flags);
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
