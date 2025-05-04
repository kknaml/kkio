module;

// #include <cstdint>
// #include <liburing.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netdb.h>

// import kkio.coro_base;
// import kkio.coro.task;
// import kkio.uring.awaiters;
// import kkio.uring.base_awaiter;
// import kkio.net.address;
// import kkio.net.netutils;
// import kkio.traits;
// import std;

export module kkio.net.tcpstream;

export namespace kkio::net {

    // class TcpStream;
    //
    // namespace detail {
    //
    //     struct ConnectAwaiter : uring::Connect {
    //
    //         sockaddr addr_{};
    //         int fd_;
    //
    //         ConnectAwaiter(int fd, sockaddr addr, socklen_t addrlen): uring::Connect(fd, &addr_, addrlen), addr_(addr), fd_(fd) {
    //         }
    //
    //         auto await_resume() -> TcpStream;
    //
    //     };
    //
    //     struct ReadAwaiter : uring::Read {
    //
    //         explicit ReadAwaiter(int fd, uint32_t size): uring::Read(fd, nullptr, size, 0) {
    //         }
    //
    //         auto await_resume() -> std::span<uint8_t> {
    //             if (data.result_ < 0) {
    //                 throw std::runtime_error(std::format("read failed: {}", data.result_));
    //             }
    //             return std::span{static_cast<uint8_t *>(data.buf_), static_cast<uint64_t>(data.result_)};
    //         }
    //
    //         auto await_suspend(std::coroutine_handle<> handle) noexcept -> void {
    //             setBufferSelectFlag();
    //             BaseUringAwaiter::await_suspend(handle);
    //         }
    //
    //     };
    //
    //     struct RecvAwaiter : uring::Recv {
    //         RecvAwaiter(int fd, uint32_t size, int flags = 0): uring::Recv(fd, nullptr, size, flags) {}
    //
    //         auto await_resume() -> std::span<uint8_t> {
    //             if (data.result_ < 0) {
    //                 throw std::runtime_error(std::format("read failed: {}", data.result_));
    //             }
    //             return std::span{static_cast<uint8_t *>(data.buf_), static_cast<uint64_t>(data.result_)};
    //         }
    //
    //         auto await_suspend(std::coroutine_handle<> handle) noexcept -> void {
    //             setBufferSelectFlag();
    //             BaseUringAwaiter::await_suspend(handle);
    //         }
    //     };
    //
    //     struct SendAwaiter : uring::Send {
    //         SendAwaiter(int fd, const void *buf, size_t len, int flags = 0): uring::Send(fd, buf, len, flags) {}
    //
    //         auto await_resume() -> int {
    //             // if (data.result_ < 0) {
    //             //     throw std::runtime_error(std::format("send failed: {}", data.result_));
    //             // }
    //             return data.result_;
    //         }
    //
    //     };
    // }
    //
    // class TcpStream : NonCopy {
    // private:
    //     int fd_;
    //     Endpoint peer_addr_;
    //
    // public:
    //     explicit TcpStream(int fd) : fd_(fd) {
    //         sockaddr_storage addr{};
    //         socklen_t len = sizeof(addr);
    //         if (getpeername(fd, reinterpret_cast<sockaddr*>(&addr), &len) == 0) {
    //             peer_addr_ = Endpoint(reinterpret_cast<sockaddr*>(&addr), len);
    //         }
    //     }
    //
    //     TcpStream(TcpStream &&other)  noexcept
    //     : fd_(std::exchange(other.fd_, -1)), peer_addr_(std::move(other.peer_addr_)) {}
    //
    //     TcpStream &operator=(TcpStream &&other) {
    //         if (this != &other) {
    //             this->fd_ = std::exchange(other.fd_, -1);
    //             this->peer_addr_ = std::move(other.peer_addr_);
    //         }
    //         return *this;
    //     }
    //
    //     auto innerFd() const noexcept -> int { return fd_; }
    //
    //     auto peerAddr(this auto &&self) noexcept -> auto && {
    //         return self.peer_addr_;
    //     }
    //
    //     auto read(uint32_t size, int flags = 0) const -> coro::Awaitable<std::span<uint8_t>> auto {
    //         return detail::RecvAwaiter{fd_, size, flags};
    //     }
    //
    //     auto write(std::span<const uint8_t> data, int flags = 0) const noexcept -> coro::Awaitable<int> auto {
    //         return detail::SendAwaiter{fd_, data.data(), data.size(), flags};
    //     }
    //
    //     // TODO readv writev
    //
    //     auto writeFully(std::span<uint8_t> data) const noexcept -> coro::Task<int> {
    //         int totalSent = 0;
    //         while (totalSent < data.size()) {
    //             auto span = std::span{data.data() + totalSent, data.size() - totalSent};
    //             auto sent = co_await write(span);
    //             if (sent <= 0) {
    //                 co_return sent;
    //             }
    //             totalSent += sent;
    //         }
    //         co_return totalSent;
    //     }
    //
    //     auto close() {
    //         int fd = std::exchange(fd_, -1);
    //         if (fd >= 0) {
    //             closeSocket(fd);
    //         }
    //     }
    //
    //     ~TcpStream() noexcept {
    //         close();
    //     }
    //
    // public:
    //     static auto connect(std::string_view host, int port) -> coro::Task<TcpStream> {
    //         addrinfo hints{}, *res{}, *rp{};
    //         int fd{};
    //
    //         hints.ai_family = AF_UNSPEC;
    //         hints.ai_socktype = SOCK_STREAM;
    //         hints.ai_flags = 0;
    //         hints.ai_protocol = 0;
    //
    //         if (getaddrinfo(host.data(), std::to_string(port).c_str(), &hints, &res) != 0) {
    //             throw std::runtime_error(std::format("Failed to resolve hostname: {}:{}", host, port));
    //         }
    //
    //         for (rp = res; rp != nullptr; rp = rp->ai_next) {
    //             fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    //             if (fd == -1) continue;
    //             try {
    //                 co_return co_await detail::ConnectAwaiter{fd, *rp->ai_addr, static_cast<socklen_t>(rp->ai_addrlen)};
    //             } catch (std::exception &e) {
    //                 closeSocket(fd);
    //                 continue;
    //             }
    //         }
    //         throw std::runtime_error(std::format("Failed to connect to {}:{}", host, port));
    //     }
    // };
    //
    // namespace detail {
    //
    //     TcpStream ConnectAwaiter::await_resume() {
    //         auto res = Connect::await_resume();
    //         if (res < 0) {
    //             throw std::runtime_error(std::format("connect failed: {}", res));
    //         }
    //         return TcpStream{fd_};
    //     }
    // } // namespace detail
} // namespace kkio::net
