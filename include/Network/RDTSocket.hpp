#ifndef RDT_MASK_HPP_
#define RDT_MASK_HPP_

#include <memory>
#include <chrono>
#include <sys/poll.h>

#include "UdpSocket.hpp"

#define HASH_BYTE_SIZE            64
#define PACKET_TYPE_BYTE_SIZE     1
#define ALTERBIT_BYTE_SIZE        1
#define MSG_BYTE_SIZE             3
#define ALTERBIT_LOWERBOUND       1
#define ALTERBIT_UPPERBOUND       3
#define RDT_HEADER_BYTE_SIZE      (HASH_BYTE_SIZE + PACKET_TYPE_BYTE_SIZE + ALTERBIT_BYTE_SIZE + MSG_BYTE_SIZE)
#define TIMEOUT                   0
#define ERROR_TIMER               -1
#define MAX_ACK_RAID_INTENTS      10

namespace rdt {

  class RDTListener;

  class RDTSocket{
  private:
    class RDTPacket{
    public:
      enum Type { Starter,
                  Acknowledgement,
                  Information,
                  Finalizer };
    private:
      Type packetType;
      std::string msgBody;
      bool corrupted;
      uint16_t ack;
    public:
      std::string encode(const std::string& message, const uint16_t& currentACK, const Type& interpretation);
      void decode(const std::string& encoded);
      bool isCorrupted();
      bool isSynchronized(const uint8_t& expectedACK);

      const Type& getPacketType();
      const std::string& getMessageBody();
      const uint16_t& getACK();
    };

    class Connection{
    public:
      std::string localIp = "<ANY>", remoteIp = "<NONE>";
      uint16_t localPort = 0, remotePort = 0;
    };

    typedef struct pollfd SocketPool;
    SocketPool sPool[1]; 
    uint16_t alterBit, lastAlterBit;

    std::unique_ptr<net::UdpSocket> mainSocket;
    Connection connectionInfo;
    net::Status connectionStatus;
    RDTPacket::Type restrictedPacketType;

    uint16_t switchBitAlternate();

    net::Status secureSend(std::string& packet);
    net::Status secureRecv(std::string& packet, const RDTPacket::Type& pType);

    bool existMessagesWaiting();
    void setTimerConfigurations();

    net::Status bindPort(const uint16_t& localPort);
    const uint16_t& getLocalPort() const;
    void resetAlterBit();
    void synchronizeACKs(const RDTSocket& other);
    void setCurrentPacketType(const RDTPacket::Type& pType);
    void disconnect();
  public:
    RDTSocket();
    ~RDTSocket();
    ///\brief Initialize a connection with an active Listener.
    ///\param remoteIp The listener (a.k.a. server) Ip address.
    ///\param remotePort The listener (a.k.a. server) port where it is bound.
    ///\return Socket Status -> Done if everything was ok.
    net::Status connect(const std::string& remoteIp, const uint16_t& remotePort);
    ///\brief Sends a message with reliable data transfer encoding to the connected socket mirror.
    ///\param message Literal or Valued variable string to send.
    ///\return Socket Status -> Done if everything was ok.
    net::Status send(const std::string& message);
    ///\brief Receives a message with reliable data transfer encoding from the connected socket mirror.
    ///\param message Referenced variable string to receive.
    ///\return Socket Status -> Done if everything was ok.
    net::Status receive(std::string& message);
    ///Checks if the socket is connected or not
    ///\return True, if the socket is online.
    ///\return False, if the socket was disconnected.
    bool online();
    ///Initialize disconnection to the receptor
    void disconnectInitializer();
    ///Wait for an incomming disconnection
    void passiveDisconnect();

    const std::string& getRemoteIpAddress() const;
    const uint16_t& getRemotePort() const;
    int32_t getSocketFileDescriptor() const;

    friend class net::UdpSocket;
    friend class RDTListener;
  };

}

std::ostream& operator<<(std::ostream& out, const rdt::RDTSocket& socket);

#endif //RDT_MASK_HPP_