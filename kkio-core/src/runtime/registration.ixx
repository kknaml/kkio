
export module kkio.runtime.registration;

import std;
import kkio.traits;
import kkio.op;
import kkio.io.util.io;



export namespace kkio::runtime {

    class Registration : NonCopy {


        template<IOOP OP, typename T>
        auto asyncIO(T &&io) -> decltype(auto) {
            return io::util::asyncIO<OP>(std::forward<T>(io));
        }
    };

}  // namespace kkio::runtime
