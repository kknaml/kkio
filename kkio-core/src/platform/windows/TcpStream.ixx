module;

import kkio.traits;
import kkio.net.ipaddr;
import kkio.io.util.iodata;

export module kkio.windows.tcpstream;

using kkio::net::IpAddr;
using kkio::io::util::IOData;

export namespace kkio::windows {

    class TcpStream : NonCopy {

        static auto connect(IpAddr &addr, int port) -> IOData;
    };

} // namespace kkio::windows
