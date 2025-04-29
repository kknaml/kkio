
module;

import kkio.net.tcpstream;

export module kkio.tls.tlsstream;

export namespace kkio::tls { 


    class TlsStream {
        private:
            kkio::net::TcpStream rawStream_;
      
    };
} // namespace kkio::tls
