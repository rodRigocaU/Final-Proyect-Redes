#include "Network/Socket.hpp"

int main(){
  RDT::UdpSocket listener("", "8000");
  RDT::UdpSocket slaveServerSocket("34.68.1.20", "8000");
  
  std::string received_message, IP_from;
  uint16_t Port_from;

  // recibes el mensaje desde el cliente
  listener.receive(received_message, IP_from, Port_from);
  // envías el mensaje al servidor esclavo
  slaveServerSocket.send(received_message);

  return 0;
}