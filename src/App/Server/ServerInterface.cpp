#include "App/Server/ServerInterface.hpp"
#include "App/Tools/Colors.hpp"
#include <thread>


app::ServerMaster::ServerMaster(const uint16_t& listenerPortClient, const uint16_t& listenerPortRepository){
  tool::ConsolePrint("[SERVER MASTER <Init Client Listener>]:", CYAN);
  if(clientListener.listen(listenerPortClient) != net::Status::Done){
    tool::ConsolePrint("=================================== (--FAILURE--)", RED);
    exit(EXIT_FAILURE);
  }
  tool::ConsolePrint("[SERVER MASTER <Init Repo. Listener>]:", CYAN);
  if(repositoryListener.listen(listenerPortRepository) != net::Status::Done){
    tool::ConsolePrint("=================================== (--FAILURE--)", RED);
    exit(EXIT_FAILURE);
  }
  tool::ConsolePrint("=================================== (--SUCCESS--)", GREEN);
  tool::ConsolePrint(">> Client Listener:", CYAN);
  std::cout << clientListener << std::endl;
  tool::ConsolePrint(">> Repository Listener:", CYAN);
  std::cout << repositoryListener << std::endl;
  tool::ConsolePrint("=================================================", VIOLET);
}

void app::ServerMaster::connEnvironmentClient(std::shared_ptr<rdt::RDTSocket>& socket){
  std::string message;
  while(socket->online()){
    socket->receive(message);
  }
}

void app::ServerMaster::connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket>& socket){
  while(socket->online()){

  }
}

void app::ServerMaster::runRepositoryListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newRepositoryIntent;
    newRepositoryIntent = std::make_shared<rdt::RDTSocket>();
    std::cout << "=>[SERVER MASTER <Spam>]: Waiting for a new connection..." << std::endl;
    if(repositoryListener.accept(*newRepositoryIntent) == net::Status::Done){
      repositoryConnectionPool[newRepositoryIntent->getSocketFileDescriptor()] = newRepositoryIntent;
      tool::ConsolePrint("=>[SERVER MASTER <Spam>]: New client connected.", CYAN);
      std::cout << *newRepositoryIntent << std::endl;
    }
  }
}

void app::ServerMaster::run(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newClientIntent;
    newClientIntent = std::make_shared<rdt::RDTSocket>();
    std::cout << "=>[SERVER MASTER <Spam>]: Waiting for a new connection..." << std::endl;
    if(clientListener.accept(*newClientIntent) == net::Status::Done){
      clientConnectionPool[newClientIntent->getSocketFileDescriptor()] = newClientIntent; 
      tool::ConsolePrint("=>[SERVER MASTER <Spam>]: New client connected.", CYAN);
      std::cout << *newClientIntent << std::endl;
    }
  }
}
