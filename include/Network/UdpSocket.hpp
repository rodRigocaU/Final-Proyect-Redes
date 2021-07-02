#ifndef UDP_SOCKET_HPP_
#define UDP_SOCKET_HPP_
#include <iostream>
#include <string>

#define LOCAL_HOST "127.0.0.1"
#define BROADCAST  "255.255.255.255"

namespace rdt{
  class RDTSocket;
}

namespace net{
  const size_t MAX_DGRAM_SIZE = 1000;

  enum Status{Done, Connected, Disconnected, Error};

  class UdpSocket{
  private:
    int32_t socketId;
    std::string localIp;
    uint16_t localPort;
  public:
    UdpSocket();
    ~UdpSocket();
    Status bind(const uint16_t& localPort);
    void unbind();
    Status send(const std::string& message, const std::string& remoteAddress, const uint16_t& remotePort);
    Status receive(std::string& message, std::string& remoteAddress, uint16_t& remotePort);

    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;
    UdpSocket& operator=(UdpSocket&&) = delete;

    friend class rdt::RDTSocket;
  };

}

#endif//UDP_SOCKET_HPP_