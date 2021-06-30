#include "Network/RDTSocket.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "App/Tools/Fixer.hpp"
#include <iomanip>

#include <math.h>

rdt::RDTSocket::RDTSocket(){
  resetAlterBit();
  connectionStatus = net::Status::Disconnected;
}

rdt::RDTSocket::~RDTSocket(){
  disconnect();
}

const uint16_t& rdt::RDTSocket::getLocalPort() const{
  return connectionInfo.localPort;
}

const std::string& rdt::RDTSocket::getRemoteIpAddress() const{
  return connectionInfo.remoteIp;
}

const uint16_t& rdt::RDTSocket::getRemotePort() const{
  return connectionInfo.remotePort;
}

int32_t rdt::RDTSocket::getSocketFileDescriptor() const{
  if(mainSocket != nullptr)
    return mainSocket->socketId;
  return -1;
}

uint16_t rdt::RDTSocket::switchBitAlternate(){
  lastAlterBit = alterBit;
  uint16_t tempMod = ALTERBIT_UPPERBOUND + 1 - ALTERBIT_LOWERBOUND;
  alterBit = (alterBit - ALTERBIT_LOWERBOUND + 1) % tempMod;
  return alterBit += ALTERBIT_LOWERBOUND;
}

void rdt::RDTSocket::setTimerConfigurations(){
  if(mainSocket != nullptr){
    sPool[0].fd = mainSocket->socketId;
    sPool[0].events = POLLIN;
  }
}

void rdt::RDTSocket::resetAlterBit(){
  alterBit = ALTERBIT_LOWERBOUND;
  lastAlterBit = ALTERBIT_UPPERBOUND;
}

void rdt::RDTSocket::synchronizeACKs(const rdt::RDTSocket& other){
  alterBit = other.alterBit;
  lastAlterBit = other.lastAlterBit;
}

void rdt::RDTSocket::setCurrentPacketType(const RDTPacket::Type& pType){
  restrictedPacketType = pType;
}

net::Status rdt::RDTSocket::connect(const std::string& remoteIp, const uint16_t& remotePort){
  if(bindPort(0) != net::Status::Done){
    disconnect();
    return net::Status::Error;
  }
  connectionInfo.remoteIp = remoteIp;
  connectionInfo.remotePort = remotePort;
  connectionStatus = net::Status::Disconnected;
  //SENDING SOCKET CLIENT CREDENTIALS
  setCurrentPacketType(RDTPacket::Type::Starter);
  RDTPacket packer;
  std::string startedPacket = packer.encode("", alterBit, RDTPacket::Type::Starter);
  if(secureSend(startedPacket) != net::Status::Done){
    disconnect();
    return net::Status::Error;
  }
  //RECEIVING SOCKET SERVER CREDENTIALS
  std::string remoteSockMirrorPort;
  if(secureRecv(remoteSockMirrorPort, RDTPacket::Type::Starter) != net::Status::Done){
    disconnect();
    return net::Status::Error;
  }
  connectionInfo.remotePort = std::stoi(remoteSockMirrorPort);
  connectionStatus = net::Status::Connected;
  setCurrentPacketType(RDTPacket::Type::Information);
  return net::Status::Done;
}

void rdt::RDTSocket::disconnect(){
  if(mainSocket != nullptr){
    mainSocket->unbind();
    mainSocket.reset();
    resetAlterBit();
    connectionStatus = net::Status::Disconnected;
  }
}

net::Status rdt::RDTSocket::bindPort(const uint16_t& localPort){
  if(mainSocket == nullptr)
    mainSocket = std::make_unique<net::UdpSocket>();
  if(mainSocket->bind(localPort) != net::Status::Done)
    return net::Status::Error;
  connectionInfo.localIp = mainSocket->localIp;
  connectionInfo.localPort = mainSocket->localPort;
  setTimerConfigurations();
  return net::Status::Done;
}

bool rdt::RDTSocket::existMessagesWaiting(){
  return sPool[0].revents & POLLIN;
}

net::Status rdt::RDTSocket::secureSend(std::string& packet) {
  if(mainSocket != nullptr){
    std::string remoteIp;
    uint16_t remotePort;
    bool successSending = false;

    uint32_t timeOut = 200; // TO DO: Función para calcular el RTT
    do{
      if(mainSocket->send(packet, connectionInfo.remoteIp, connectionInfo.remotePort) != net::Status::Done)
        return net::Status::Error;

      int32_t responseTimeCode = poll(sPool, 1, timeOut);
      if(responseTimeCode == ERROR_TIMER)
        return net::Status::Error;
      else if(responseTimeCode == TIMEOUT)
        continue;
      else {
        if(existMessagesWaiting()) {
          std::string ACKPacket;
          RDTPacket packer;
          mainSocket->receive(ACKPacket, remoteIp, remotePort);
          packer.decode(ACKPacket);
          if(!packer.isCorrupted()){
            if(packer.getPacketType() == RDTPacket::Type::Acknowledgement && packer.isSynchronized(alterBit))
              successSending = true;
            else{
              if(packer.isSynchronized(lastAlterBit)){
                std::string ACK = packer.encode("", packer.getACK(), RDTPacket::Type::Acknowledgement);
                if(mainSocket->send(ACK, remoteIp, remotePort) != net::Status::Done)
                  return net::Status::Error;
              }
              else
                break;
            }
          }
        }
      }
    } while(!successSending);
    switchBitAlternate();
    return net::Status::Done;
  }
  return net::Status::Disconnected;
}

net::Status rdt::RDTSocket::secureRecv(std::string& packet, const RDTPacket::Type& pType){
  if(mainSocket != nullptr){
    std::string remoteIp;
    uint16_t remotePort;
    bool successReceiving = false;
    std::size_t bytes_sent;
    do {
      mainSocket->receive(packet, remoteIp, remotePort);
      if(connectionStatus == net::Status::Disconnected){
        connectionInfo.remoteIp = remoteIp;
        connectionInfo.remotePort = remotePort;
      }
      RDTPacket packer;
      packer.decode(packet);
      packet = packer.getMessageBody();
      if(!packer.isCorrupted()){
        std::string ACK;
        if(packer.isSynchronized(alterBit))
          successReceiving = true;
        ACK = packer.encode("", packer.getACK(), RDTPacket::Type::Acknowledgement);
        if(mainSocket->send(ACK, connectionInfo.remoteIp, connectionInfo.remotePort) != net::Status::Done)
          return net::Status::Error;
      }
    } while (!successReceiving);
    switchBitAlternate();
    return net::Status::Done;
  }
  return net::Status::Disconnected;
}

net::Status rdt::RDTSocket::send(const std::string& message){
  net::Status commStatus;
  const uint64_t BODY_MSG_BYTE_SIZE = net::MAX_DGRAM_SIZE - RDT_HEADER_BYTE_SIZE;
  uint64_t packetCount = std::ceil(double(message.length()) / double(BODY_MSG_BYTE_SIZE));
  RDTPacket packer;
  std::string packetCountEncoded = packer.encode(std::to_string(packetCount), alterBit, RDTPacket::Type::Information);
  if((commStatus = secureSend(packetCountEncoded)) != net::Status::Done)
    return commStatus;

  for(uint64_t i = 0, j = 0; i < packetCount; ++i, j += BODY_MSG_BYTE_SIZE) {
    std::string packetChunk = packer.encode(message.substr(j, BODY_MSG_BYTE_SIZE), alterBit, restrictedPacketType);
    if((commStatus = secureSend(packetChunk)) != net::Status::Done)
      return commStatus;
  }
  return net::Status::Done; 
}

net::Status rdt::RDTSocket::receive(std::string& message){
  message.clear();
  net::Status commStatus;
  std::string nSubPackets;
  if((commStatus = secureRecv(nSubPackets, RDTPacket::Type::Information)) != net::Status::Done)
    return commStatus;
  uint64_t packetCount = std::stoi(nSubPackets);
  for(uint64_t i = 0; i < packetCount; ++i){
    std::string packetChunk;
    if((commStatus = secureRecv(packetChunk, restrictedPacketType)) != net::Status::Done)
      return commStatus;
    message += packetChunk;
  }
  return net::Status::Done;
}

void rdt::RDTSocket::disconnectInitializer(){
  RDTPacket packer;
  std::string finalizerPacket = packer.encode("", alterBit, RDTPacket::Type::Finalizer);
  setCurrentPacketType(RDTPacket::Finalizer);
  secureSend(finalizerPacket);
  disconnect();
}

void rdt::RDTSocket::passiveDisconnect(){
  std::string finalizerPacketRecv;
  secureRecv(finalizerPacketRecv, RDTPacket::Type::Finalizer);
  secureRecv(finalizerPacketRecv, RDTPacket::Type::Finalizer);
  disconnect();
}

std::ostream& operator<<(std::ostream& out, const rdt::RDTSocket& socket){
  out << "+--------------------+\n";
  out << "|RDT::Reliable Socket|\n";
  out << "+--------------------+\n";
  out << "|FD: " << std::setw(16) << socket.getSocketFileDescriptor() << "|\n";
  out << "|Ip: " << std::setw(16) << socket.getRemoteIpAddress() << "|\n";
  out << "|Port: " << std::setw(14) << socket.getRemotePort() << "|\n";
  out << "+--------------------+\n";
  return out;
}