
module;

#include <liburing.h>

import std;
import kkio.traits;
import kkio.coro.cancellation;

export module kkio.uring.uring;

using kkio::coro::CancellationToken;

export namespace kkio::uring {

    class Ring final : NonCopy {
    private:
        io_uring inner{};
        uint8_t **bufs{};
        io_uring_buf_ring *br{};
        int bufs_in_group{};
    public:
        explicit Ring() noexcept;

        ~Ring() noexcept;

        auto get_ring() noexcept -> io_uring *;

        auto init(uint32_t entries, uint32_t flag, uint32_t bufs_in_group) -> void;

        auto get_sqe() -> io_uring_sqe *;

        auto submit() noexcept -> int;

        auto wait(io_uring_cqe **cqe, std::optional<__kernel_timespec> timeout = std::nullopt) -> int;

        auto wait_nr(io_uring_cqe **cqe, uint32_t nr) -> int;

        auto seen(io_uring_cqe *cqe) -> void;

        auto get_buffer(int id) noexcept -> uint8_t *;

        auto cancel(CancellationToken &token, void *user_data, int flag) -> void;

        static auto current() -> Ring &;
        static auto set_current(Ring *ring) -> void;
    };

}
