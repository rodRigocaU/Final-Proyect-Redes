#include "Network/RDTMask.hpp"
#include "Network/UdpSocket.hpp"
#include "App/Server/ServerInterface.hpp"

class someSocket {
  net::UdpSocket socket;
  public:
  someSocket() {}
  someSocket(const std::string& IP, const std::string& Port)
  {
    socket.configureSocket(IP, Port);
  }
  void conf(const std::string& IP, const std::string& Port)
  {
    socket.configureSocket(IP, Port);
  }
  int sendAll(u_char *buffer, int &bytes_sent, bool to_sender) {
    socket.sendAll(buffer, bytes_sent, to_sender);
  }
  int simpleRecv(u_char *buffer) {
    socket.simpleRecv(buffer);
  }

  std::string getSenderIP() {
    return socket.getSenderIP();
  }
  uint16_t getSenderPort() {
    return socket.getSenderPort();
  }
};

int main(){

  //MainServerApp MainServer; 
  rdt::RDTSocket mainServerListener("", 8000);
  //mainServerListener.setReceptorSettings("", 8000);
  std::string received_msg, ip_from;
  uint16_t port_from;
  
  mainServerListener.receive(received_msg, ip_from, port_from);
  
  std::cout << "some new message:\n";
  std::cout << "recv_msg: "<< received_msg << "\n";
  std::cout << "IPfrom: " << ip_from << "\n";
  std::cout << "port from: " << port_from << "\n";


  //net::UdpSocket serverSimpleSocket;
  // someSocket serverSimpleSocket;
  // serverSimpleSocket.conf("", "8000");
  // // serverSimpleSocket.configureSocket("", "8000");
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
