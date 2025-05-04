module;

// #ifdef WIN32
// #include <Windows.h>
//
// #else
//
// #endif


export module kkio.kkfd;

export namespace kkio {

#ifdef WIN32
    using FdType = void *;

#else
    using FdType = int;
#endif

    struct KKFd {
        FdType inner_;
        KKFd(FdType fd) noexcept : inner_(fd) {}

        operator FdType() const noexcept { return inner_; }
    };

} // namespace kkio