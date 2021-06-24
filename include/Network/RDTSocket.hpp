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
  class RDTListener;

  class RDTSocket{
  private:
    class Connection{
    public:
      std::string localIp = "<ANY>", remoteIp;
      uint16_t localPort = 0, remotePort;
    };

    typedef struct pollfd SocketTimer;
    SocketTimer timer[1]; 
    uint8_t alterBit;

    std::unique_ptr<net::UdpSocket> mainSocket;
    Connection connectionInfo, lastConnectionIntent;
    net::Status connectionStatus;

    uint8_t switchBitAlternate();
    std::string encode(const std::string& message);
    bool decode(std::string& message);

    net::Status secureSend(std::string& packet, const uint8_t& expectedAlterBit);
    net::Status secureRecv(std::string& packet, const uint8_t& expectedAlterBit);

    bool existMessagesWaiting();
    void setTimerConfigurations();

    net::Status bindPort(const uint16_t& localPort);
  public:
    RDTSocket();
    ~RDTSocket();
    net::Status connect(const std::string& remoteIp, const uint16_t& remotePort);
    void disconnect();
    net::Status send(const std::string& message);
    net::Status receive(std::string& message);

    friend class net::UdpSocket;
    friend class RDTListener;
  };

}

#endif //RDT_MASK_HPP_