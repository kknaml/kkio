module;

import std;
import kkio.traits;
import kkio.ioutil;
import kkio.uring.awaiter.all;

export module kkio.fs.file;

export namespace kkio::fs {

    class File final : public NonCopy {
    private:
        int fd;
        explicit File(int fd) noexcept : fd(fd) {}

    public:
        File(File &&other) noexcept : fd(std::exchange(other.fd, -1)) {}
        File &operator=(File &&other) noexcept {
            if (this != &other) {
                auto old = std::exchange(other.fd, -1);
                if (old >= 0) { close_fd(fd); }
            }
            return *this;
        }

        ~File() {
            auto fd = std::exchange(this->fd, -1);
            if (fd >= 0) {
                close_fd(fd);
            }
        }

        auto read(std::span<uint8_t> buf, int offset = 0) -> uring::Read {
            return uring::Read(this->fd, buf.data(), buf.size(), offset);
        }

        auto write(std::span<const uint8_t> buf, int offset) -> uring::Write {
            return uring::Write(this->fd, buf.data(), buf.size(), offset);
        }

        auto get_fd() const noexcept -> int { return fd; }

        auto take_fd() noexcept -> int {
            return std::exchange(this->fd, -1);
        }


    public:
        static auto from_fd(int fd) noexcept { return File{fd}; }
        static auto open(std::string_view file, int flag) -> File {
            return File{open_fd(file.data(), flag)};
        }
    };
}