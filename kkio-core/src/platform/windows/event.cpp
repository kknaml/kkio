module kkio.windows.event;

import kkio.windows.afd;

namespace kkio::windows {

    auto token(const Event &event) noexcept -> Token {
        return reinterpret_cast<Token>(event.data_);
    }

    Event::Event(Token token) noexcept : flags_(0), data_(reinterpret_cast<void *>(token)) {}

    auto Event::setReadable() noexcept -> void {
        flags_ |= afd::POLL_RECEIVE;
    }

    auto Event::setWritable() noexcept -> void {
        flags_ |= afd::POLL_SEND;
    }
} //namespace kkio::windows