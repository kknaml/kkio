module;

#include <liburing.h>
#include <cstdlib>
#include <cstdio>

import std;
import kkio.uring.iodata;

module kkio.uring.uring;

namespace {

    using kkio::uring::Ring;

    thread_local Ring *local_ring{nullptr};

    constexpr int BUF_BGID_1 = 0;

    const auto page_size = getpagesize();

    auto allocate_bufs(int bufs_in_group) -> uint8_t ** {
        auto bufs = new uint8_t *[bufs_in_group];
        for (auto i = 0; i < bufs_in_group; i++) {
            bufs[i] = new uint8_t[page_size];
        }
        return bufs;
    }

    auto free_bufs(uint8_t **bufs, int bufs_in_group) -> void {
        for (auto i = 0; i < bufs_in_group; i++) {
            delete[] bufs[i];
        }
        delete[] bufs;
    }

    auto setup_buffer_ring(
        io_uring *ring,
        int bufs_in_group
    ) -> std::tuple<io_uring_buf_ring *, uint8_t **> {
        io_uring_buf_reg reg{};
        io_uring_buf_ring *br{};

        uint8_t **bufs = allocate_bufs(bufs_in_group);
        if (posix_memalign(
                reinterpret_cast<void **>(&br),
                page_size,
                bufs_in_group * sizeof(io_uring_buf_ring)
            )
        ) {
            throw std::bad_alloc();
        }
        reg.ring_addr = reinterpret_cast<uint64_t>(br);
        reg.ring_entries = bufs_in_group;
        reg.bgid = BUF_BGID_1;

        if (io_uring_register_buf_ring(ring, &reg, 0)) {
            free(br);
            throw std::runtime_error("io_uring_register_buf_ring() failed");
        }

        io_uring_buf_ring_init(br);
        for (auto i = 0; i < bufs_in_group; i++) {
            io_uring_buf_ring_add(br, bufs[i], 4096, i, io_uring_buf_ring_mask(bufs_in_group), i);
        }

        io_uring_buf_ring_advance(br, bufs_in_group);

        return std::make_tuple(br, bufs);
    }

    auto free_buffer_ring(
        io_uring *ring,
        uint8_t **bufs,
        int bufs_in_group
    ) -> void {
        // TODO
    }
}

namespace kkio::uring {

    Ring::Ring() noexcept {
        local_ring = this;
    }

    Ring::~Ring() noexcept {
        if (local_ring != this) {
            std::println(stderr, "Local Ring Mismatch!");
        } else {
            local_ring = nullptr;
        }
        io_uring_queue_exit(&this->inner);
    }

    auto Ring::get_ring() noexcept -> io_uring * {
        return &this->inner;
    }

    auto Ring::init(uint32_t entries, uint32_t flag, uint32_t bufs_in_group) -> void {
        auto ret = io_uring_queue_init(entries, &this->inner, flag);
        if (ret != 0) {
            auto err = -ret;
            throw std::system_error(err, std::system_category(), "io_uring_queue_init ");
        }
        auto [br, bufs] = setup_buffer_ring(&this->inner, bufs_in_group);
        this->br = br;
        this->bufs = bufs;
    }

    auto Ring::get_sqe() -> io_uring_sqe * {
        return io_uring_get_sqe(&this->inner);
    }

    auto Ring::submit() noexcept -> int {
        return io_uring_submit(&this->inner);
    }

    auto Ring::wait(io_uring_cqe **cqe, std::optional<__kernel_timespec> timeout) -> int {
        if (timeout.has_value()) {
            return io_uring_wait_cqe_timeout(&this->inner, cqe, &*timeout);
        } else {
            return io_uring_wait_cqe(&this->inner, cqe);
        }
    }

    auto Ring::wait_nr(io_uring_cqe **cqe, uint32_t nr) -> int {
        return io_uring_wait_cqe_nr(&this->inner, cqe, nr);
    }

    auto Ring::seen(io_uring_cqe *cqe) -> void {
        io_uring_cqe_seen(&this->inner, cqe);
    }

    auto Ring::get_buffer(int id) noexcept -> uint8_t * {
        return bufs[id];
    }

    auto Ring::cancel(CancellationToken &token, void *user_data, int flag) -> void {
        auto sqe = this->get_sqe();
        // TODO

        static_cast<IOData *>(user_data)->cancel_token = &token;
        io_uring_prep_cancel(sqe, user_data, flag);
        this->submit();
    }

    auto Ring::current() -> Ring & {
#ifdef KKIO_DEBUG
        if (local_ring == nullptr) {
            std::println(stderr, "Local Ring Not Set!");
            std::abort();
        }
#endif
        return  *local_ring;
    }
}
