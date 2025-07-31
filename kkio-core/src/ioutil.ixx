module;

import std;
import kkio.uring.awaiter.all;

export module kkio.ioutil;

export namespace kkio {

    auto close_fd(int fd) noexcept -> int;

    auto open_fd(const char *file, int flag) noexcept -> int;

}