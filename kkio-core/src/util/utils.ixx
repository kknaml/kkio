module;

#include <cstdint>

import std;

export module kkio.util.utils;

export namespace kkio::util {

    auto printHex(std::span<const uint8_t> data) -> void {
        std::cout << std::hex << std::uppercase;
        for (const auto byte : data) {
            std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::nouppercase << std::endl;
    }
    
} // namespace kkio::util
