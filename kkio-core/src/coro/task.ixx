module;

#include <cstdio>

import std;
import kkio.coro.cancellation;

export module kkio.coro.task;

export namespace kkio::coro {

    template<typename T = void>
    class Task;

    namespace detail {

        struct TaskFinalAwaiter {
            constexpr auto await_ready() const noexcept -> bool { return false; }

            template<typename Promise>
            auto await_suspend(std::coroutine_handle<Promise> current) const noexcept -> std::coroutine_handle<> {
                auto &promise = current.promise();
                if (promise.exception) {
                    try {
                        std::rethrow_exception(promise.exception);
                    } catch (CancellationException &e) {
                        // task was canceled
                        if (promise.parent) {
                            promise.parent.promise().exception = promise.exception;
                            return promise.parent;
                        }
                    } catch (std::exception &e) {
                        std::println(stderr, "task exception:{}", e.what());
                    } catch (...) {
                        std::println(stderr, "task unknown error!");
                    }
                }
                if (promise.parent) {
                    return promise.parent;
                }
                // root task
                current.destroy();
                return std::noop_coroutine();
            }

            constexpr auto await_resume() const noexcept -> void {}
        };

        template<typename Promise>
        struct BaseTaskPromise {
            std::coroutine_handle<Promise> parent{nullptr};
            std::exception_ptr exception{nullptr};
            std::shared_ptr<CancellationToken> cancel_token{nullptr};

            constexpr auto initial_suspend() const noexcept -> std::suspend_never {
                return {};
            }

            constexpr auto final_suspend() const noexcept -> TaskFinalAwaiter {
                return {};
            }


            auto unhandled_exception() noexcept -> void {
                this->exception = std::current_exception();
            }

            auto check_error() const {
                if (this->exception) {
                    std::rethrow_exception(this->exception);
                }
            }

            auto cancel(std::string msg) noexcept -> bool {
                if (!cancel_token) return false;
                return cancel_token->cancel(std::move(msg));
            }

        };

        template<typename T>
        struct TaskPromise final : BaseTaskPromise<TaskPromise<T>> {
            std::optional<T> value{std::nullopt};

            auto get_return_object() noexcept -> Task<T>;

            auto return_value(T t) {
                this->value = std::move(t);
            }

            auto get_value() & -> T & {
                this->check_error();
                return this->value.value();
            }

            auto get_value() && -> T {
                this->check_error();
                return std::move(this->value.value());
            }
        };

        template<>
        struct TaskPromise<void> final : BaseTaskPromise<TaskPromise<void>> {
            auto get_return_object() noexcept -> Task<>;

            auto return_void() noexcept -> void {

            }

            auto get_value() const -> void {
                this->check_error();
            }
        };
    }


    template<typename T>
    class Task {
    public:
        using promise_type = detail::TaskPromise<T>;

    private:
        std::coroutine_handle<promise_type> handle;

    public:

        explicit Task(std::coroutine_handle<promise_type> h) noexcept : handle(h) {}

        auto get_handle() const noexcept -> std::coroutine_handle<promise_type> {
            return handle;
        }

        auto take_handle() noexcept -> std::coroutine_handle<promise_type> {
            return std::exchange(handle, nullptr);
        }

        ~Task() {
            auto h = std::exchange(handle, nullptr);
            if (h) {
                h.destroy();
            }
        }
    };

    namespace detail {

        template<typename T>
        auto TaskPromise<T>::get_return_object() noexcept -> Task<T> {
            return Task<T>(std::coroutine_handle<TaskPromise>::from_promise(*this));
        }

        auto TaskPromise<void>::get_return_object() noexcept -> Task<> {
            return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
        }

    }
}
