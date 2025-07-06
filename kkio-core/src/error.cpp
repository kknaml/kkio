module;


import std;

module kkio.error;


namespace kkio {

    auto get_error_msg(int error_no) noexcept -> std::string {
        if (error_no < 0) {
            error_no = -error_no;
        }
        if (error_no != 0 && error_no < 9999999) { // TODO
            std::error_code ec(error_no, std::generic_category());
            return ec.message();
        }

        return std::format("Unknown error code {}", error_no);
    }
}
