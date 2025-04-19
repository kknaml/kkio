module;

import std;

export module kkio.looper;

export namespace kkio {

    class Looper {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<std::coroutine_handle<>> coro_queue;
        bool running = false;

    public:
        Looper() = default;
        ~Looper() {
            stop();
        }

        void start() {
            std::unique_lock lock(mtx);
            if (running) return;
            running = true;
            
            while (running) {
                if (coro_queue.empty()) {
                    cv.wait(lock, [this] { return !coro_queue.empty() || !running; });
                    if (!running) break;
                }

                auto handle = coro_queue.front();
                coro_queue.pop();
                lock.unlock();

                if (handle) {
                    handle.resume();
                }

                lock.lock();
            }
        }

        void stop() {
            std::unique_lock lock(mtx);
            if (!running) return;
            running = false;
            cv.notify_all();
        }


        void post(std::coroutine_handle<> handle) {
            std::unique_lock lock(mtx);
            coro_queue.push(handle);
            cv.notify_one();
        }


        bool isRunning() const {
            return running;
        }
    };

} // namespace kkio
