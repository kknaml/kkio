
export module kkio.io.util.io;

import std;
import kkio.op;



export namespace kkio::io::util {

    template<typename TRead>
    struct AsyncRead {
        static auto asyncRead(TRead &&read) -> decltype(auto) {
            static_assert(false, "Read is not implemented for this type");
        }
    };

    template<typename TRead>
    auto asyncRead(TRead &&read) -> decltype(auto) {
        return AsyncRead<TRead>::asyncRead(std::forward<TRead>(read));
    }

    template<typename TWrite>
    struct AsyncWrite {
        static auto asyncWrite(TWrite &&write) -> decltype(auto) {
            static_assert(false, "Write is not implemented for this type");
        }
    };

    template<typename TWrite>
    auto asyncWrite(TWrite &&write) -> decltype(auto) {
        return AsyncWrite<TWrite>::asyncWrite(std::forward<TWrite>(write));
    }

    template<typename TAccept>
    struct AsyncAccept {
        static auto asyncAccept(TAccept &&accept) -> decltype(auto) {
            static_assert(false, "Accept is not implemented for this type");
        }
    };

    template<typename TAccept>
    auto asyncAccept(TAccept &&accept) -> decltype(auto) {
        return AsyncAccept<TAccept>::asyncAccept(std::forward<TAccept>(accept));
    }

    template<typename TConnect>
    struct AsyncConnect {
        static auto asyncConnect(TConnect &&connect) -> decltype(auto) {
            static_assert(false, "Connect is not implemented for this type");
        }
    };

    template<typename TConnect>
    auto asyncConnect(TConnect &&connect) -> decltype(auto) {
        return AsyncConnect<TConnect>::asyncConnect(std::forward<TConnect>(connect));
    }

    template<typename TSend>
    struct AsyncSend {
        static auto asyncSend(TSend &&send) -> decltype(auto) {
            static_assert(false, "Send is not implemented for this type");
        }
    };

    template<typename TSend>
    auto asyncSend(TSend &&send) -> decltype(auto) {
        return AsyncSend<TSend>::asyncSend(std::forward<TSend>(send));
    }
    
    template<typename TRecv>
    struct AsyncRecv {
        static auto asyncRecv(TRecv &&recv) -> decltype(auto) {
            static_assert(false, "Recv is not implemented for this type");
        }
    };
    
    template<typename TRecv>
    auto asyncRecv(TRecv &&recv) -> decltype(auto) {
        return AsyncRecv<TRecv>::asyncRecv(std::forward<TRecv>(recv));
    }
    
    template<typename TTimeout>
    struct AsyncTimeout {
        static auto asyncTimeout(TTimeout &&timeout) -> decltype(auto) {
            static_assert(false, "Timeout is not implemented for this type");
        }
    };

    template<typename TTimeout>
    auto asyncTimeout(TTimeout &&timeout) -> decltype(auto) {
        return AsyncTimeout<TTimeout>::asyncTimeout(std::forward<TTimeout>(timeout));
    }

    template<IOOP OP>
    struct AsyncIO {

        template<typename IO>
        static auto asyncIO(IO &&io) -> decltype(auto) {
            if constexpr (OP == IOOP::READ) {
                return asyncRead(std::forward<IO>(io));
            } else if constexpr (OP == IOOP::WRITE) {
                return asyncWrite(std::forward<IO>(io));
            } else if constexpr (OP == IOOP::ACCEPT) {
                return asyncAccept(std::forward<IO>(io));
            } else if constexpr (OP == IOOP::CONNECT) {
                return asyncConnect(std::forward<IO>(io));
            } else if constexpr (OP == IOOP::SEND) {
                return asyncSend(std::forward<IO>(io));
            } else if constexpr (OP == IOOP::RECV) {
                return asyncRecv(std::forward<IO>(io));
            } else if constexpr (OP == IOOP::TIMEOUT) {
                return asyncTimeout(std::forward<IO>(io));
            } else {
                static_assert(false, "Unsupported operation");
            }
        }
    };

    template<IOOP OP, typename IO>
    auto asyncIO(IO &&io) -> decltype(auto) {
        return AsyncIO<OP>::asyncIO(std::forward<IO>(io));
    }

} // namespace kkio::io::util