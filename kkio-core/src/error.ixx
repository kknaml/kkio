module;

import std;

export module kkio.error;

export namespace kkio {


    class Error {
    public:
        int code{};
        std::string message{};
    };

    template<typename T>
    using Result = std::expected<T, Error>;

    auto get_error_msg(int error_no) noexcept -> std::string;
}