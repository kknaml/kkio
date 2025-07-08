module;

import std;
import kkio.traits;
import kkio.net.sock_addr;

export module kkio.net.stream;

export namespace kkio::net {

    template<typename T>
    concept SockStream = requires(T &t) {
        { t.get_fd() } -> std::same_as<int>;
    };

    class TCPListener final : NonCopy {
    private:
        int fd;

    public:
        explicit TCPListener(int fd) noexcept : fd(fd) {}
        T

        auto get_fd() const noexcept -> int;
    };
}
