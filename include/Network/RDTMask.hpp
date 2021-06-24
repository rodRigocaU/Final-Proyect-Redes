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
  public:
    enum Status {Done, Wait, Disconnected, Error};
  private:
    typedef struct pollfd SocketTimer;
    uint8_t alterBit = ALTERBIT_LOWERBOUND;
    net::UdpSocket mainSocket;
    SocketTimer timer[1]; 

    uint8_t switchBitAlternate();
    uint8_t getBitAlternate(u_char* buffer);
    bool isCorrupted(const std::string& message, const std::string& hash);
    std::string encode(const std::string& message);
    bool decode(std::string& message);

    Status secureSend(std::string& packet, const uint8_t& expectedAlterBit);
    Status secureRecv(std::string& packet, const uint8_t& expectedAlterBit);

  public:
    void setReceptorSettings(const std::string& IpAddress, const uint16_t& port);
    Status send(const std::string& message);
    Status receive(std::string& message, std::string& remoteIp, uint16_t& remotePort);
  };

}

#endif //RDT_MASK_HPP_