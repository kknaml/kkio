
module;

import std;
import kkio.kkfd;
import kkio.platform;
import kkio.op;

export module kkio.io.util.iodata;

export namespace kkio::io::util {

    struct IOData {
        KKFd fd_;
        IOOP op_;
        std::coroutine_handle<> coroutineHandle_{nullptr};
        platform::IODataInner inner_{};
    };
}
