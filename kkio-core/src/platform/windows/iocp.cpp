
#include <Windows.h>
#include <cstdio>

import std;
import kkio.windows.iocp;
import kkio.traits;
import kkio.windows.event;

namespace kkio::windows::iocp {

    namespace {
        auto throwIocpError(std::string_view type) -> void {
            auto error = GetLastError();
            auto msg = std::format("{} failed {}\n{}", type, error, std::stacktrace::current());
            std::println(stderr, "{}", msg);
            throw std::runtime_error(msg);
        }
    }


    Overlapped::Overlapped(void (*callback)(OVERLAPPED_ENTRY *, std::optional<std::vector<Event>>)) noexcept : inner_{}, callback_(callback) {
    }

    auto Overlapped::operator->() const -> OVERLAPPED * {
        return &inner_;
    }

    CompletionPort::CompletionPort(uint32_t size) {
        auto ret = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, size);
        if (ret == nullptr) {
            throwIocpError("CreateIoCompletionPort");
        }
        handle_ = ret;
    }



    auto CompletionPort::addHandle(size_t token, HANDLE handle) const -> void {
        auto ret = CreateIoCompletionPort(handle, handle_, token, 0);
        if (ret == nullptr) {
            throwIocpError("CreateIoCompletionPort");
        }
    }

    auto CompletionPort::getMany(
        std::span<CompletionStatus> list,
        Duration timeout
    ) const -> std::span<CompletionStatus> {
        uint32_t removed = 0;
        auto len = list.size();
        auto ret = GetQueuedCompletionStatusEx(
            handle_,
            reinterpret_cast<OVERLAPPED_ENTRY *>(list.data()),
            len,
            reinterpret_cast<PULONG>(&removed),
            timeout.count(),
            0
        );
        if (ret == 0) {
            throwIocpError("GetQueuedCompletionStatusEx");
        }
        return std::span {list.data(), removed};
    }

    auto CompletionPort::post(const CompletionStatus &status) const -> void {
        auto ret = PostQueuedCompletionStatus(
            handle_,
            status->dwNumberOfBytesTransferred,
            status->lpCompletionKey,
            status->lpOverlapped
        );
        if (ret == 0) {
            throwIocpError("PostQueuedCompletionStatus");
        }
    }

    CompletionStatus::CompletionStatus(uint32_t bytes, size_t token, Overlapped *overlapped) noexcept {
        overlappedEntry_ = {
            .lpCompletionKey = token,
            .lpOverlapped = &overlapped->inner_,
            .Internal = 0,
            .dwNumberOfBytesTransferred = bytes,
        };
    }

    auto CompletionStatus::bytesTransferred() const noexcept -> uint32_t {
        return (*this)->dwNumberOfBytesTransferred;
    }

    auto CompletionStatus::token() const noexcept -> uint64_t {
        return (*this)->lpCompletionKey;
    }

    auto CompletionStatus::overlapped() noexcept -> OVERLAPPED * {
        return (*this)->lpOverlapped;
    }

    auto CompletionStatus::fromEntry(OVERLAPPED_ENTRY *entry) -> CompletionStatus * {
        return reinterpret_cast<CompletionStatus *>(entry);
    }

    auto CompletionStatus::zero() -> CompletionStatus {
        return CompletionStatus(0, 0, nullptr);
    }
} // namespace kkio::windows::iocp
