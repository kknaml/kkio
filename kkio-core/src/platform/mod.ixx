
export module kkio.platform;

import kkio.windows.iodata;

export namespace kkio::platform {

    struct TODO{};

#ifdef WIN32
    using Selector = TODO;
    using Event = TODO;
    using IODataInner = windows::IODataWindows;
#else
    using Selector = TODO;
    using Event = TODO;
#endif




} // namespace kkio::platform