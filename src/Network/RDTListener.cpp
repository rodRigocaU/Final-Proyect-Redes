#include "Network/RDTListener.hpp"
#include <iomanip>

net::Status rdt::RDTListener::listen(const uint16_t& listenPort){
  listenSocket.setCurrentPacketType(RDTSocket::RDTPacket::Type::Starter);
  return listenSocket.bindPort(listenPort);
}

net::Status rdt::RDTListener::accept(RDTSocket& incomingConnection){
  std::string connectionGretting;
  listenSocket.resetAlterBit();
  if(incomingConnection.bindPort(0) != net::Status::Done){
    incomingConnection.disconnect();
    return net::Status::Error;
  }
  //RECEIVING SOCKET CLIENT CREDENTIALS
  if(listenSocket.receive(connectionGretting) != net::Status::Done)
    return net::Status::Error;
  incomingConnection.connectionInfo.remoteIp = listenSocket.connectionInfo.remoteIp;
  incomingConnection.connectionInfo.remotePort = listenSocket.connectionInfo.remotePort;

  //SENDING SOCKET SERVER CREDENTIALS
  if(listenSocket.send(std::to_string(incomingConnection.getLocalPort())) != net::Status::Done){
    return net::Status::Error;
  }

  incomingConnection.synchronizeACKs(listenSocket);
  incomingConnection.connectionStatus = net::Status::Connected;
  incomingConnection.setCurrentPacketType(RDTSocket::RDTPacket::Type::Information);
  return net::Status::Done;
}

void rdt::RDTListener::close(){
  listenSocket.disconnect();
}

const uint16_t& rdt::RDTListener::getLocalPort() const{
  return listenSocket.getLocalPort();
}

std::ostream& operator<<(std::ostream& out, const rdt::RDTListener& listener){
  out << "+--------------------+\n";
  out << "|RDT::Listener       |\n";
  out << "+--------------------+\n";
  out << "|Port: " << std::setw(14) << listener.getLocalPort() << "|\n";
  out << "+--------------------+\n";
  return out;
}