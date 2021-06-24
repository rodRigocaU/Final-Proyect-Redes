#include "Network/RDTListener.hpp"

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