
module;

#include <Windows.h>

export module kkio.windows.iocp;

import std;
import kkio.traits;
import kkio.windows.event;

export namespace kkio::windows::iocp {

    struct Overlapped {
        mutable OVERLAPPED inner_;
        void (*callback_)(OVERLAPPED_ENTRY *, std::optional<std::vector<Event>>);

        explicit Overlapped(void (*callback)(OVERLAPPED_ENTRY *, std::optional<std::vector<Event>>)) noexcept;

        auto operator->() const -> OVERLAPPED *;
    };

    struct CompletionStatus;

    struct CompletionPort {
        HANDLE handle_;

        explicit CompletionPort(HANDLE handle) noexcept : handle_(handle) {}

        explicit CompletionPort(uint32_t size);

        auto addHandle(size_t token, HANDLE handle) const -> void;

        auto getMany(
            std::span<CompletionStatus> list,
            Duration timeout
        ) const -> std::span<CompletionStatus>;

        auto post(const CompletionStatus &status) const -> void;
    };

    struct CompletionStatus {
        OVERLAPPED_ENTRY overlappedEntry_;

        explicit CompletionStatus(OVERLAPPED_ENTRY overlappedEntry) : overlappedEntry_(overlappedEntry) {}

        CompletionStatus(
            uint32_t bytes,
            size_t token,
            Overlapped *overlapped
        ) noexcept;

        auto operator->(this auto &&self) -> auto {
            return &self.overlappedEntry_;
        }

        auto bytesTransferred() const noexcept -> uint32_t;

        auto token() const noexcept -> uint64_t;

        auto overlapped() noexcept -> OVERLAPPED *;

        static auto fromEntry(OVERLAPPED_ENTRY *entry) -> CompletionStatus *;
        static auto zero() -> CompletionStatus;
    };

}  // namespace kkio::windows::iocp

