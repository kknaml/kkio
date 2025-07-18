module;

import std;
import kkio.traits;

export module kkio.tls.extensions;

export namespace kkio::tls {

    struct TLSExtension {
        uint16_t type;
        std::vector<uint8_t> data;
    };

}