
module;

import std;
import kkio.kkfd;
import kkio.platform;

export module kkio.io.util.iodata;

export namespace kkio::io::util {

    struct IOData {
        KKFd fd_;
        std::coroutine_handle<> coroutineHandle_{};
        platform::IODataInner inner_{};
    };
}
