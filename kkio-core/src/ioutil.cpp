module;

#include <fcntl.h>
#include <unistd.h>

import std;

module kkio.ioutil;

namespace kkio {
    auto close_fd(int fd) noexcept -> int {
        return close(fd);
    }

    auto open_fd(const char *file, int flag) noexcept -> int {
        return open(file, flag);
    }
}