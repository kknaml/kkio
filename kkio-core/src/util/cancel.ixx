module;

import std;


export module kkio.util.cancel;


export namespace kkio::util {

    class CancellationException : public std::runtime_error {

        explicit CancellationException(const char* message) : runtime_error(message) {}
    };

} // namespace kkio::util
