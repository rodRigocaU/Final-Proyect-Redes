#include "Network/RDTListener.hpp"
#include <iomanip>

net::Status rdt::RDTListener::listen(const uint16_t& listenPort){
  return listenSocket.bindPort(listenPort);
}

net::Status rdt::RDTListener::accept(rdt::RDTSocket& incomingConnection){
  std::string connectionGretting, localConfirmedPort;
  incomingConnection.mainSocket = std::make_unique<net::UdpSocket>();
  incomingConnection.setTimerConfigurations();
  //RECEIVING SOCKET CLIENT CREDENTIALS
  if(listenSocket.receive(connectionGretting) != net::Status::Done)
    return net::Status::Error;
  if(connectionGretting != "PLOX")
    return net::Status::Error;
  incomingConnection.connectionInfo.remoteIp = listenSocket.connectionInfo.remoteIp;
  incomingConnection.connectionInfo.remotePort = listenSocket.connectionInfo.remotePort;
  
  //SENDING SOCKET SERVER CREDENTIALS
  if(incomingConnection.send("PASS") != net::Status::Done){
    incomingConnection.disconnect();
    return net::Status::Error;
  }
  incomingConnection.connectionStatus = net::Status::Connected;
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