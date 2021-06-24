#include "App/Server/ServerInterface.hpp"

namespace app{
  
  ServerMaster::ServerMaster(const uint16_t& listenerPort){
    if(listener.listen(listenerPort) != net::Status::Done){
      exit(EXIT_FAILURE);
    }
    std::cout << "[SERVER MASTER]\n";
    std::cout << listener << std::endl;
  }

  void ServerMaster::run(){
    while(true){
      std::shared_ptr<rdt::RDTSocket> newClientIntent;
      newClientIntent = std::make_shared<rdt::RDTSocket>();
      if(listener.accept(*newClientIntent) == net::Status::Done){
        clientConnectionPool[newClientIntent->getSocketFileDescriptor()] = newClientIntent; 
      }
    }
  }

}