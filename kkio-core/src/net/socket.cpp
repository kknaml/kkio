module;

#include <fcntl.h>
#include <unistd.h>

import std;

module kkio.net.socket;

namespace kkio::net {
    auto detail::BaseSocket::is_alive() const noexcept -> bool {
        return fcntl(fd, F_GETFD) != -1;
    }

    TcpStream::TcpStream(int fd) noexcept : detail::BaseSocket(fd) {
    }

    TcpStream::TcpStream(TcpStream &&other) noexcept : detail::BaseSocket(std::exchange(other.fd, -1)) {

    }

    auto TcpStream::operator=(TcpStream &&other) noexcept -> TcpStream & {
        if (this != &other) {
            auto fd = std::exchange(this->fd, -1);
            if (fd >= 0) {
                close(fd);
            }
            this->fd = std::exchange(other.fd, -1);
        }
        return *this;
    }

    TcpStream::~TcpStream() {
        auto fd = std::exchange(this->fd, -1);
        if (fd >= 0) {
            close(fd);
        }
    }
}
