module;

import std;

export module kkio.error;

export namespace kkio {


    class Error {
    public:
        int code{};
        std::string message{};

        static auto from(int code) noexcept -> Error;
    };

    template<typename T>
    using Result = std::expected<T, Error>;

    auto get_error_msg(int error_no) noexcept -> std::string;

    constexpr int ERR_NO_SQE = 10086;

    constexpr std::array KKIO_PREDEFINED_ERRORS = {
        std::pair<int, std::string_view> {ERR_NO_SQE, "NO SQE available"},
    };

    constexpr std::string_view get_predefined_error_msg(int error_no) noexcept {
        for (const auto &err : KKIO_PREDEFINED_ERRORS) {
            if (error_no == err.first) {
                return err.second;
            }
        }
        return "UNKNOWN KKIO ERROR";
    }

}