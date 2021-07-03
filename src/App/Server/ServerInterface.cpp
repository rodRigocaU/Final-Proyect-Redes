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
  onlineRepositoriesCount = 0;
}

void app::ServerMaster::connEnvironmentClient(std::shared_ptr<rdt::RDTSocket>& socket){
  std::string message;
  if(socket->online()){
    socket->receive(message);
    /*
      Process task
    */
    socket->passiveDisconnect();
  }
}

void app::ServerMaster::connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket>& socket){
  IdNetNode repositoryId = REPOSITORY_LIMIT + socket->getSocketFileDescriptor();
  uint16_t remoteListenPort;
  std::string message;
  socket->receive(message);
  remoteListenPort = std::stoi(message);
  repoInteractionMutex.lock();
  repositoryConnectionPool[repositoryId] = {remoteListenPort, socket};
  repoInteractionMutex.unlock();
  while(socket->online()){
    socket->receive(message);
    if(message[0] == COMMAND_RENAME){
      repoInteractionMutex.lock();
      std::pair<uint16_t, std::shared_ptr<rdt::RDTSocket>> temp = repositoryConnectionPool[repositoryId];
      repositoryConnectionPool.erase(repositoryId);
      repositoryConnectionPool[(repositoryId = std::stoull(message.substr(1)))] = temp;
      repoInteractionMutex.unlock();
    }
    else if(message[0] == COMMAND_LINK){
      repoTaskQueueMutex.lock();
      message = message.substr(1);
      std::pair<IdNetNode, std::list<IdNetNode>> commandBody = tool::parseRepoActiveCommand(message);
      preProcessRepositoryQueue.push({COMMAND_LINK, commandBody.first, commandBody.second});
      repoTaskQueueMutex.unlock();
    }
    else if(message[0] == COMMAND_DETACH){
      repoTaskQueueMutex.lock();
      message = message.substr(1);
      std::pair<IdNetNode, std::list<IdNetNode>> commandBody = tool::parseRepoActiveCommand(message);
      preProcessRepositoryQueue.push({COMMAND_DETACH, commandBody.first, commandBody.second});
      repoTaskQueueMutex.unlock();
    }
  }
}

void app::ServerMaster::runRepositoryListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newRepositoryIntent;
    newRepositoryIntent = std::make_shared<rdt::RDTSocket>();
    alternateConsolePrintMutex.lock();
    std::cout << "=>[SERVER MASTER <Spam>]: Waiting for a new repository..." << std::endl;
    alternateConsolePrintMutex.unlock();
    if(repositoryListener.accept(*newRepositoryIntent) == net::Status::Done){
      repositoryConnectionPool[REPOSITORY_LIMIT + newRepositoryIntent->getSocketFileDescriptor()] = {0, newRepositoryIntent};
      ++onlineRepositoriesCount;
      std::thread clientThread(&ServerMaster::connEnvironmentRepository, this, newRepositoryIntent);
      alternateConsolePrintMutex.lock();
      tool::ConsolePrint("=>[SERVER MASTER <Spam>]: New repository connected.", CYAN);
      std::cout << *newRepositoryIntent << std::endl;
      alternateConsolePrintMutex.unlock();
    }
  }
}

void app::ServerMaster::run(){
  std::thread repositoryThread(&ServerMaster::runRepositoryListener, this);
  while(true){
    std::shared_ptr<rdt::RDTSocket> newClientIntent;
    newClientIntent = std::make_shared<rdt::RDTSocket>();
    alternateConsolePrintMutex.lock();
    std::cout << "=>[SERVER MASTER <Spam>]: Waiting for a new connection..." << std::endl;
    alternateConsolePrintMutex.unlock();
    if(clientListener.accept(*newClientIntent) == net::Status::Done){
      clientConnectionPool[newClientIntent->getSocketFileDescriptor()] = newClientIntent;
      std::thread clientThread(&ServerMaster::connEnvironmentClient, this, newClientIntent);
      clientThread.detach();
      alternateConsolePrintMutex.lock();
      tool::ConsolePrint("=>[SERVER MASTER <Spam>]: New client connected.", CYAN);
      std::cout << *newClientIntent << std::endl;
      alternateConsolePrintMutex.unlock();
    }
  }
  repositoryThread.join();
}
