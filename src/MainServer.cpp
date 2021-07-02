#include "Network/RDTSocket.hpp"
#include "Network/RDTListener.hpp"
#include "App/Server/ServerInterface.hpp"



int main(){
  rdt::RDTListener listener;
  if(listener.listen(5001) != net::Status::Done){
    return EXIT_FAILURE;
  }
  
  rdt::RDTSocket socket;
  if(listener.accept(socket) != net::Status::Done){
    return EXIT_FAILURE;
  }
  
  
 
  /*net::UdpSocket slaveServerSocket("35.188.208.43", "8000");

  std::string received_message, IP_from;
  uint16_t Port_from;
  mainServerListener.receive(received_message, IP_from, Port_from);
  slaveServerSocket.send(received_message);*/
  /*
  while(true){
    std::unique_ptr<RDTSocket> newSocket = std::make_unique<RDTSocket>();
  }    
  */
  return 0;
}
