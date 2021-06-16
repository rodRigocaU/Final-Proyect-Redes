#ifndef RDT_MASK_HPP_
#define RDT_MASK_HPP_

#include <memory>
#include <sys/poll.h>

#include "UdpSocket.hpp"

namespace rdt
{  
  /*
    00000001 + 00100101 + 0996 + message
    alter + checksum + msg_size + message

    Delete RdtEncoder & Decoder turning them to methods in RDTSocket [v]
  */
  class RDTSocket{
  private:
    std::unique_ptr<net::UdpSocket> mainSocket;
    
    uint8_t switchBitAlternate();

    u_char getChecksum(const std::string& message);
    
    void encode(std::string& message);
    
    void decode(std::string& message); 

  public:
    void send(const std::string& message);
    void receive();
  };

}

#endif //RDT_MASK_HPP_