module;

#include <Windows.h>

export module kkio.windows.iodata;

export namespace kkio::windows {

    struct IODataWindows {
        OVERLAPPED overlapped_;
    };
}
