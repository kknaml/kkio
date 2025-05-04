export module kkio.op;


export namespace kkio {

    enum class IOOP {
        READ,
        WRITE,
        ACCEPT,
        CONNECT,
        SEND,
        RECV,
        TIMEOUT,
    };

}