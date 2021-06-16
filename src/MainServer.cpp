#include "Network/UdpSocket.hpp"
#include "Server/ServerInterface.hpp"

int main(){
  MainServerApp MainServer; 
  net::UdpSocket mainServerListener("", "8000");
  net::UdpSocket slaveServerSocket("35.188.208.43", "8000");
  std::string received_message, IP_from;
  uint16_t Port_from;
  mainServerListener.receive(received_message, IP_from, Port_from);
  slaveServerSocket.send(received_message);
  return 0;
}
