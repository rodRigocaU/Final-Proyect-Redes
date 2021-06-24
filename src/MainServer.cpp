#include "Network/RDTMask.hpp"
#include "Network/UdpSocket.hpp"
#include "App/Server/ServerInterface.hpp"

int main(){

  //MainServerApp MainServer; 
  rdt::RDTSocket mainServerListener;
  mainServerListener.setReceptorSettings("", 8000);
  std::string received_msg, ip_from;
  uint16_t port_from;

  mainServerListener.receive(received_msg, ip_from, port_from);
  std::cout << "some new message:\n";
  std::cout << ""<< received_msg << "\n";
  std::cout << ip_from << "\n";
  std::cout << port_from << "\n";


  // net::UdpSocket serverSimpleSocket;
  // serverSimpleSocket.configureSocket("", "8000");
  // //net::UdpSocket serverSimpleSocket("", "8000");

  // u_char message[net::MAX_DGRAM_SIZE];
  // // serverSimpleSocket->simpleRecv(message);
  // serverSimpleSocket.simpleRecv(message);
  // std::cout << message << "\n";
  // // std::cout <<"from: " << serverSimpleSocket->getSenderIP() << "\n";
  // std::cout <<"from: " << serverSimpleSocket.getSenderIP() << "\n";
  // // std::cout <<"on port: " << serverSimpleSocket->getSenderPort() << "\n";
  // std::cout <<"on port: " << serverSimpleSocket.getSenderPort() << "\n";

  /*net::UdpSocket slaveServerSocket("35.188.208.43", "8000");
  std::string received_message, IP_from;
  uint16_t Port_from;
  mainServerListener.receive(received_message, IP_from, Port_from);
  slaveServerSocket.send(received_message);*/
  return 0;
}
