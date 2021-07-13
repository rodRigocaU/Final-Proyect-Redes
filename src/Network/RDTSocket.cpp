#include "App/Tools/Fixer.hpp"
#include "Network/RDTSocket.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "Network/Algorithm/RDTEstimator.hpp"
#include <chrono>
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

    std::chrono::high_resolution_clock::time_point t_pollStart, t_pollEnd;
    uint32_t estimatedRTT;
    while(!successSending){
      if(mainSocket->send(packet, connectionInfo.remoteIp, connectionInfo.remotePort) != net::Status::Done){
        return net::Status::Error;
      }
      t_pollStart = std::chrono::high_resolution_clock::now();
      int32_t responseTimeCode = poll(sPool, 1, estimatedRTT);
      t_pollEnd = std::chrono::high_resolution_clock::now();
      estimatedRTT = ewmaEstimator.estimate(
                      std::chrono::duration_cast<std::chrono::milliseconds>(t_pollEnd - t_pollStart).count());

      if(responseTimeCode == ERROR_TIMER) {
        return net::Status::Error;
      }
      else if(responseTimeCode == TIMEOUT)
        continue;
      else {
        if(existMessagesWaiting()) {
          std::string ACKPacket;
          RDTPacket packer;
          mainSocket->receive(ACKPacket, remoteIp, remotePort);
          packer.decode(ACKPacket);
          if(!packer.isCorrupted()){
            //CASE 1: Normal situation, ACK received correctly
            if(packer.isSynchronized(alterBit) && packer.getPacketType() == RDTPacket::Type::Acknowledgement)
              successSending = true;
            //CASE 2: Packet with past ACK waiting to receive an ACK yet
            else if(packer.isSynchronized(lastAlterBit) && 
                    packer.getPacketType() != RDTPacket::Type::Acknowledgement){
              std::string ACK = packer.encode("", packer.getACK(), RDTPacket::Type::Acknowledgement);
              if(mainSocket->send(ACK, remoteIp, remotePort) != net::Status::Done)
                return net::Status::Error;
            }
            //CASE 3 : Packet with the next ACK after the current one, same as CASE 1
            else if(packer.getPacketType() != RDTPacket::Type::Acknowledgement)
              successSending = true;
          }
        }
      }
    }
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
    while (!successReceiving){
      mainSocket->receive(packet, remoteIp, remotePort);
      if(connectionStatus == net::Status::Disconnected){
        connectionInfo.remoteIp = remoteIp;
        connectionInfo.remotePort = remotePort;
      }
      RDTPacket packer;
      packer.decode(packet);
      if(!packer.isCorrupted()){
        packet = packer.getMessageBody();
        std::string ACK;
        //CASE 1: Normal situation, Packet with correct ACK received
        if(packer.isSynchronized(alterBit) && packer.getPacketType() != RDTPacket::Type::Acknowledgement)
          if(packer.getPacketType() == pType)
            successReceiving = true;
        //CASE 2: Received an ACK of any other receive call function, unexpected and ignored
        //        Receive function only accepts Packets with not ACK flags.
        else if(packer.getPacketType() == RDTPacket::Type::Acknowledgement)
          continue;
        //CASE 1 & 3: Sending ACK for a Packet, no matters if this one has a correct ACK
        ACK = packer.encode("", packer.getACK(), RDTPacket::Type::Acknowledgement);
        if(mainSocket->send(ACK, remoteIp, remotePort) != net::Status::Done)
          return net::Status::Error;
      }
    }
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
  std::string packetCountEncoded = packer.encode(std::to_string(packetCount), alterBit, 
                                                 RDTPacket::Type::Information);
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

bool rdt::RDTSocket::online(){
  return (mainSocket != nullptr) && (connectionStatus == net::Connected);
}

void rdt::RDTSocket::disconnectInitializer(){
  RDTPacket packer;
  std::string finalizerPacket = packer.encode("", alterBit, RDTPacket::Type::Finalizer);
  setCurrentPacketType(RDTPacket::Finalizer);
  secureSend(finalizerPacket);
  disconnect();
}

void rdt::RDTSocket::passiveDisconnect(){
  std::string finalizerPacketRecv, ACK_Intent;
  secureRecv(finalizerPacketRecv, RDTPacket::Type::Finalizer);
  RDTPacket packer;
  ACK_Intent = packer.encode("", lastAlterBit, RDTPacket::Type::Acknowledgement);
  for (int32_t i = 0; i < MAX_ACK_RAID_INTENTS; ++i){
    mainSocket->send(ACK_Intent, connectionInfo.remoteIp, connectionInfo.remotePort);
  }
  disconnect();
}

std::ostream& operator<<(std::ostream& out, const rdt::RDTSocket& socket){
  out << "+----------------------+\n";
  out << "|RDT::Reliable Socket  |\n";
  out << "+----------------------+\n";
  out << "|FD: " << std::setw(18) << socket.getSocketFileDescriptor() << "|\n";
  out << "|RemIp: " << std::setw(15) << socket.getRemoteIpAddress() << "|\n";
  out << "|RemPort: " << std::setw(13) << socket.getRemotePort() << "|\n";
  out << "+----------------------+\n";
  return out;
}