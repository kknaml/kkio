
export module kkio.windows.event;

import kkio.traits;
import std;

export namespace kkio::windows {

    using Token = uint64_t;

    struct Event {
        uint32_t flags_;
        void *data_;

        explicit Event(Token token) noexcept;

        auto setReadable() noexcept -> void;

        auto setWritable() noexcept -> void;
    };

    auto token(const Event &event) noexcept -> Token;

} // namespace kkio::windows
