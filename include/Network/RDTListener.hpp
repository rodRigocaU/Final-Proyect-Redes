#ifndef RDT_LISTENER_HPP_
#define RDT_LISTENER_HPP_

#include "RDTSocket.hpp"

namespace rdt{

  class RDTListener{
  private:
    RDTSocket listenSocket;
  public:
    net::Status listen(const uint16_t& listenPort);
    net::Status accept(RDTSocket& incomingConnection);
    void close();

    friend class RDTSocket;
  };

}

#endif//RDT_LISTENER_HPP_