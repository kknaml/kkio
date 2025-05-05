export module kkio.coro.context;

import std;

export namespace kkio::coro {

    template<typename T>
    struct Element;

    struct CoroutineContextKey {
        constexpr CoroutineContextKey() = default;
        virtual ~CoroutineContextKey() = default;

        [[nodiscard]]
        virtual auto type() const -> std::type_index = 0;
    };

    struct CoroutineContext : std::enable_shared_from_this<CoroutineContext> {
        using Ptr = std::shared_ptr<CoroutineContext>;

        virtual ~CoroutineContext() = default;

        virtual auto operator [](const CoroutineContextKey &key) const -> Ptr = 0;

        virtual auto operator +(Ptr other) -> Ptr = 0;

        template<typename T>
        // requires std::derived_from<typename T::KeyType, CoroutineContextKey>
        // && std::default_initializable<typename T::KeyType>
        auto get() const -> std::shared_ptr<Element<T>> {
            // auto key = T::KeyType{};
            // return std::dynamic_pointer_cast<T>((*this)[key]);
            using KeyType = typename Element<T>::KeyType;
            auto key = KeyType{};
            return std::dynamic_pointer_cast<Element<T>>((*this)[key]);
        }
    };

    template<typename T>
    struct Element : CoroutineContext {
        struct KeyType : CoroutineContextKey {
            constexpr KeyType() noexcept : CoroutineContextKey(){}
            auto type() const -> std::type_index override {
                return typeid(T);
            }
        };
        T value_;
        Ptr next_;

        explicit Element(T value, Ptr n = nullptr) : value_(std::move(value)), next_(n) {}

        auto operator [](const CoroutineContextKey &key) const -> Ptr override {
            if (key.type() == typeid(T)) {
                return std::const_pointer_cast<Element>(std::static_pointer_cast<const Element>(shared_from_this()));
            }
            return next_ ? (*next_)[key] : nullptr;
        }

        auto operator +(Ptr other) -> Ptr override {
            if (!other) return shared_from_this();
            return std::make_shared<Element>(value_, other);
        }

        auto operator ->() noexcept -> T * {
            return &value_;
        }
    };

    struct EmptyContext final : CoroutineContext {
        auto operator [](const CoroutineContextKey &key) const -> Ptr override;
        auto operator +(Ptr other) -> Ptr override;
        auto operator ==(std::nullptr_t) const noexcept -> bool;

        static auto instance() -> Ptr;

        constexpr EmptyContext() noexcept = default;
    };

    namespace detail {
        struct CoroutineContextAwaiter {
            CoroutineContext::Ptr ptr_{};
            constexpr auto await_ready() const noexcept -> bool {
                return false;
            }

            template<typename Promise>
            auto await_suspend(std::coroutine_handle<Promise> handle) noexcept -> bool {
                ptr_ = handle.promise().context();
                return false;
            }

            auto await_resume() const noexcept -> CoroutineContext::Ptr {
                return ptr_;
            }
        };
    }

    [[nodiscard("co_await")]]
    constexpr auto currentCoroutineContext() noexcept -> detail::CoroutineContextAwaiter {
        return {};
    }

} // namespace kkio::coro