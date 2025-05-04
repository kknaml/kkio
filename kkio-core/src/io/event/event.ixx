export module kkio.io.event;

import std;
import kkio.traits;
import kkio.platform;

export namespace kkio::io::event {

    struct Event : NonCopy {
        platform::Event inner_;
    };
}