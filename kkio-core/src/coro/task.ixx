module;

#include <cstdio>

import std;
import kkio.coro_base;
import kkio.traits;

export module kkio.task;


namespace kkio::coro {

    export template <typename T = void>
    class Task;

    namespace detail {
        struct FinalTaskAwaiter {
            constexpr auto await_ready() const noexcept -> bool { return false; }

            template<typename Promise>
            auto await_suspend(std::coroutine_handle<Promise> callee) const noexcept -> std::coroutine_handle<> {
                auto &callee_promise = callee.promise();
                if (callee_promise.ex_) {
                    try {
                        std::rethrow_exception(callee_promise.ex_);
                    } catch (std::exception &e) {
                        std::println(stderr, "task exception: {}", e.what());
                    } catch (...) {
                        std::println(stderr, "task unknown exception");
                    }
                }
                if (callee_promise.caller_) {
                    return callee_promise.caller_;
                }
                callee.destroy();
                return std::noop_coroutine();
            }

            constexpr auto await_resume() const noexcept {}
        };

        template<typename Promise>
        struct BaseTaskAwaiter {
            std::coroutine_handle<Promise> callee_;

            auto await_ready() const noexcept -> bool {
                return !callee_ || callee_.done();
            }

            template<typename Promise2>
            auto await_suspend(std::coroutine_handle<Promise2> caller) -> std::coroutine_handle<> {
                callee_.promise().caller_ = caller;
                return callee_;
            }
        };

        struct BaseTaskPromise {
            std::coroutine_handle<> caller_ {nullptr};
            std::exception_ptr ex_ {nullptr};

            constexpr auto initial_suspend() const noexcept -> std::suspend_always {
                return {};
            }

            auto final_suspend() const noexcept -> FinalTaskAwaiter {
                return {};
            }

            auto unhandled_exception() noexcept {
                this->ex_ = std::current_exception();
            }

            auto check_error() const {
                if (ex_) {
                    std::rethrow_exception(ex_);
                }
            }

            template<typename Awaiter>
            requires std::is_base_of_v<AwaiterMarker, std::remove_reference_t<Awaiter>>
            auto await_transform(Awaiter &&awaiter) const noexcept -> auto && {
                return std::forward<Awaiter>(awaiter);
            }

            template<typename T>
            auto await_transform(this auto &&self, Task<T> &&task) {
                struct Awaiter : BaseTaskAwaiter<typename Task<T>::promise_type> {
                    auto await_resume() -> decltype(auto) {
                        return std::move(this->callee_.promise()).getValue();
                    }
                };
                return Awaiter(task.takeHandle());
            }

            template<typename T>
            auto await_transform(this auto &&self, Task<T> &task) {
                struct Awaiter : BaseTaskAwaiter<typename Task<T>::promise_type> {
                    auto await_resume() -> decltype(auto) {
                        return this->callee_.promise().getValue();
                    }
                };
                return Awaiter(task.handle());
            }
        };

         template<typename T>
            struct TaskPromise final : BaseTaskPromise {
                std::optional<T> value_{};

                auto get_return_object() -> Task<T>;

                auto return_value(auto &&value) noexcept {
                    this->value_ = std::forward<decltype(value)>(value);
                }

                auto getValue() & -> T & {
                    check_error();
                    return value_.value();
                }

                auto getValue() && -> T {
                    check_error();
                    return std::move(value_.value());
                }

                auto hasValue() const noexcept -> bool {
                    return value_.has_value();
                }
            };

        template<>
        struct TaskPromise<void> final : BaseTaskPromise {
            bool hasValue_ {false};
            auto get_return_object() -> Task<>;

            auto return_void() noexcept {
                hasValue_ = true;
            }

            auto getValue() const {
                check_error();
            }

            auto hasValue() const noexcept -> bool {
                return hasValue_;
            }
        };

    } // namespace detail

    export template<typename T>
    class Task : public NonCopy {
    public:
        using promise_type = detail::TaskPromise<T>;
    private:
        std::coroutine_handle<promise_type> handle_;

    public:
        explicit Task(std::coroutine_handle<promise_type> handle) : handle_(handle) {}

        Task(Task &&other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}

        auto operator=(Task &&other) noexcept -> Task & {
            if (this != &other) {
                if (handle_) {
                    handle_.destroy();
                }
                handle_ = std::exchange(other.handle_, nullptr);
            }
            return *this;
        }

        ~Task() {
            if (handle_) {
                handle_.destroy();
            }
        }

        auto handle() const noexcept -> std::coroutine_handle<promise_type> {
            return handle_;
        }

        auto takeHandle() noexcept -> std::coroutine_handle<promise_type> {
            return std::exchange(handle_, nullptr);
        }
    };

    template<typename T>
    auto detail::TaskPromise<T>::get_return_object() -> Task<T> {
        return Task<T>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }

    inline auto detail::TaskPromise<void>::get_return_object() -> Task<> {
        return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }


} // namespace kkio::coro
