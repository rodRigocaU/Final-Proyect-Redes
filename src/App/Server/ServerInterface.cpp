#include "App/Server/ServerInterface.hpp"
#include "App/Tools/Colors.hpp"

namespace app{
  
  ServerMaster::ServerMaster(const uint16_t& listenerPort){
    tool::ConsolePrint("[SERVER MASTER INIT]:", CYAN);
    if(listener.listen(listenerPort) != net::Status::Done){
      tool::ConsolePrint("                     --FAILURE--", RED);
      exit(EXIT_FAILURE);
    }
    tool::ConsolePrint("                      --SUCCESS--", GREEN);
    std::cout << listener << std::endl;
  }

  void ServerMaster::run(){
    while(true){
      std::shared_ptr<rdt::RDTSocket> newClientIntent;
      newClientIntent = std::make_shared<rdt::RDTSocket>();
      if(listener.accept(*newClientIntent) == net::Status::Done){
        tool::ConsolePrint("[SERVER MASTER SPAM]: New Client Connected.", CYAN);
        std::cout << *newClientIntent << std::endl;
        clientConnectionPool[newClientIntent->getSocketFileDescriptor()] = newClientIntent; 
      }
    }
  }

}