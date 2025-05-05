
import std;
import kkio.coro.context;

namespace kkio::coro {

    auto EmptyContext::operator[](const CoroutineContextKey &key) const -> Ptr {
        return nullptr;
    }

    auto EmptyContext::operator+(Ptr other) -> Ptr {
        return other;
    }

    auto EmptyContext::operator==(std::nullptr_t) const noexcept -> bool {
        return true;
    }

    auto EmptyContext::instance() -> Ptr {
        static Ptr instance = std::make_shared<EmptyContext>();
        return instance;
    }
} // namespace kkio::coro
