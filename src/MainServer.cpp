#include "Network/RDTMask.hpp"
#include "Server/ServerInterface.hpp"

int main(){

  //MainServerApp MainServer; 
  rdt::RDTSocket mainServerListener;
  mainServerListener.setReceptorSettings("", 8000);
  std::string received_msg, ip_from;
  uint16_t port_from;

  mainServerListener.receive(received_msg, ip_from, port_from);
  std::cout << received_msg << "\n";
  std::cout << ip_from << "\n";
  std::cout << port_from << "\n";

  /*net::UdpSocket slaveServerSocket("35.188.208.43", "8000");
  std::string received_message, IP_from;
  uint16_t Port_from;
  mainServerListener.receive(received_message, IP_from, Port_from);
  slaveServerSocket.send(received_message);*/
  return 0;
}
