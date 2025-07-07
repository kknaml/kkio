module;

#include <cstdio>

import std;
import kkio.traits;
import kkio.coro.cancellation;
import kkio.coro.base;

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
                        TODO();
                        // if (promise.parent) {
                        //     promise.parent.promise().exception = promise.exception;
                        //     return promise.parent;
                        // }
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
                // current.destroy();
                // current();
                current.promise().invoke_completion_cb();
                return std::noop_coroutine();
            }

            constexpr auto await_resume() const noexcept -> void {}
        };

        template<typename Promise>
        struct BaseTaskAwaiter {
            std::coroutine_handle<Promise> current{nullptr};

            explicit BaseTaskAwaiter(std::coroutine_handle<Promise> p) noexcept : current(p) {}

            auto await_ready() const noexcept -> bool {
                return !current || current.done();
            }

            template<typename Promise2>
            auto await_suspend(std::coroutine_handle<Promise2> parent) -> std::coroutine_handle<> {
                current.promise().parent = parent;
                return current;
            }
        };

        template<typename Promise>
        struct BaseTaskPromise {
            using CompletionCB = std::function<void()>;

            std::coroutine_handle<> parent{nullptr};
            std::exception_ptr exception{nullptr};
            std::shared_ptr<CancellationToken> cancel_token{nullptr};
            std::vector<CompletionCB> completions{};

            constexpr auto initial_suspend() const noexcept -> std::suspend_always {
                return {};
            }

            constexpr auto final_suspend() const noexcept -> TaskFinalAwaiter {
                return {};
            }

            auto invoke_completion_cb() {
                auto cbs = std::move(completions);
                for (auto &cb : cbs) {
                    cb();
                }
            }

            auto invoke_on_completion(Action auto &&f) {
                if (static_cast<Promise *>(this)->has_value()) {
                    f();
                } else {
                    this->completions.emplace_back(std::forward<decltype(f)>(f));
                }
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

            template<typename T>
            auto await_transform(Task<T> &&task) {
                struct Awaiter final : BaseTaskAwaiter<typename Task<T>::promise_type> {
                    using BaseTaskAwaiter<typename Task<T>::promise_type>::BaseTaskAwaiter;
                    auto await_resume() -> decltype(auto) {
                        return std::move(this->current.promise()).get_value();
                    }
                };
                return Awaiter{task.take_handle()};
            }

            template<typename T>
            auto await_transform(Task<T> &task) {
                struct Awaiter final : BaseTaskAwaiter<typename Task<T>::promise_type> {
                    using BaseTaskAwaiter<typename Task<T>::promise_type>::BaseTaskAwaiter;
                    auto await_resume() -> decltype(auto) {
                        return this->current.promise().get_value();
                    }
                };
                return Awaiter{task.take_handle()};
            }

            auto await_transform(auto &&awaiter_like) -> decltype(auto) {
                return get_awaiter(std::forward<decltype(awaiter_like)>(awaiter_like));
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

            auto has_value() const noexcept -> bool {
                return this->value != std::nullopt;
            }
        };

        template<>
        struct TaskPromise<void> final : BaseTaskPromise<TaskPromise<void>> {
            bool value_filled{false};
            auto get_return_object() noexcept -> Task<>;

            auto return_void() noexcept -> void {
                this->value_filled = true;
            }

            auto get_value() const -> void {
                this->check_error();
            }

            auto has_value() const noexcept -> bool {
                return this->value_filled;
            }
        };
    }


    template<typename T>
    class Task final : NonCopy {
    public:
        using promise_type = detail::TaskPromise<T>;

    private:
        std::coroutine_handle<promise_type> handle;

    public:

        explicit Task(std::coroutine_handle<promise_type> h) noexcept : handle(h) {}

        Task(Task &&other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

        auto operator=(Task &&other) noexcept -> Task & {
            if (&other == this) return *this;
            this->handle = std::exchange(other.handle, nullptr);
            return *this;
        }

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
