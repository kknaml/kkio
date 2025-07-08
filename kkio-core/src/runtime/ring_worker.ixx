
module;

import std;
import kkio.traits;
import kkio.uring.uring;

export module kkio.runtime.worker;

using kkio::uring::Ring;

export namespace kkio::runtime {

    class RingWorker final : NonCopy {
    private:
        Ring ring{};
        std::mutex mutex{};
        std::condition_variable condition{};
        std::thread thread{};
        std::atomic_bool running{true};
        std::queue<std::coroutine_handle<>> queue{};
        int event_fd;
        uint64_t event_buf{};

    private:
        auto run(std::latch *latch) noexcept -> void;

        auto get_handle() noexcept -> std::coroutine_handle<>;

    public:
        RingWorker(uint32_t entries, uint32_t flag, uint32_t bufs_in_group, std::latch &latch);

        ~RingWorker() noexcept;

        auto get_ring() noexcept -> Ring &;

        auto add_handle(std::coroutine_handle<> handle) -> void;

        auto notify() noexcept -> void;

        auto shutdown() noexcept -> void;

    public:
        static auto create(
            uint32_t entries,
            uint32_t flag,
            uint32_t bufs_in_group,
            std::latch &latch
        ) -> std::unique_ptr<RingWorker>;
    };

}
