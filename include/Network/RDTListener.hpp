#ifndef RDT_LISTENER_HPP_
#define RDT_LISTENER_HPP_

#include "RDTSocket.hpp"
#include <iostream>

namespace rdt{

  class RDTListener{
  private:
    RDTSocket listenSocket;
  public:
    net::Status listen(const uint16_t& listenPort);
    net::Status accept(RDTSocket& incomingConnection);
    void close();
    const uint16_t& getLocalPort() const;
    const std::string& getLocalIp() const;

    friend class RDTSocket;
    friend class RDTPacket;
  };

}

std::ostream& operator<<(std::ostream& out, const rdt::RDTListener& listener);

#endif//RDT_LISTENER_HPP_