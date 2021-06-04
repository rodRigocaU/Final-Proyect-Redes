// MainServer.cpp
#include "Network/Socket.hpp"
int main(){
  RDT::UdpSocket mainServerListener("", "8000");
  RDT::UdpSocket slaveServerSocket("35.188.208.43", "8000");
  std::string received_message, IP_from;
  uint16_t Port_from;
  // recibes el mensaje desde el cliente
  mainServerListener.receive(received_message, IP_from, Port_from);
  // envías el mensaje al servidor esclavo
  slaveServerSocket.send(received_message);
  return 0;
}
