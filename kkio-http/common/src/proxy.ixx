module;

import std;
import kkio.traits;

export module kkio.http.proxy;


export namespace kkio::http {

    enum class ProxyType {
        DIRECT,
        HTTP,
        SOCKS
    };

    class Proxy final : NonCopy {

    };
}