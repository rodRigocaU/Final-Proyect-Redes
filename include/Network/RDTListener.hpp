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

    friend class RDTSocket;
  };

}

std::ostream& operator<<(std::ostream& out, const rdt::RDTListener& listener);

#endif//RDT_LISTENER_HPP_