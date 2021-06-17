#ifndef RDT_MASK_HPP_
#define RDT_MASK_HPP_

#include <memory>
#include <sys/poll.h>

#include "UdpSocket.hpp"

#define HASH_BYTE_SIZE        64
#define ALTERBIT_BYTE_SIZE    1
#define ALTERBIT_LOWERBOUND   1
#define ALTERBIT_UPPERBOUND   2
#define MSG_BYTE_SIZE         4
#define RDT_HEADER_BYTE_SIZE  ALTERBIT_BYTE_SIZE + HASH_BYTE_SIZE + MSG_BYTE_SIZE

namespace rdt
{  

  class RDTSocket{
  private:
    uint8_t alterBit = ALTERBIT_LOWERBOUND;
    std::unique_ptr<net::UdpSocket> mainSocket;

    uint8_t switchBitAlternate();
    bool isCorrupted(const std::string& message, const std::string& hash);
    void encode(std::string& message);
    bool decode(std::string& message); 

  public:
    enum Status {Done, Wait, Disconnected, Error};

    void setReceptorSettings(const std::string& IpAddress, const uint16_t& port);
    Status send(const std::string& message);
    Status receive(std::string& message, std::string& remoteIp, uint16_t& remotePort);
  };

}

#endif //RDT_MASK_HPP_