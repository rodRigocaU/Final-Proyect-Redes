#include "App/Server/ServerInterface.hpp"
#include "App/Tools/Colors.hpp"
#include "App/Tools/Fixer.hpp"
#include <algorithm>
#include <iterator>
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
  onlineRepositoriesCount = 0;
  tool::ConsolePrint("=================================================", VIOLET);
}

void app::ServerMaster::connEnvironmentClient(std::shared_ptr<rdt::RDTSocket> socket){
  std::string message;
  if(socket->online()){
    socket->receive(message);
    std::string dbNodeId;
    uint8_t commandKey = message[0];
    if(commandKey == 'c'){
      dbNodeId = message.substr(1);
      dbNodeId = tool::asStreamString(dbNodeId, 3);
    }
    else if(commandKey == 'r'){
      dbNodeId = message.substr(1);
      dbNodeId = tool::asStreamString(dbNodeId, 2);
    }
    else if(commandKey == 'u'){
      dbNodeId = message.substr(2);
      dbNodeId = tool::asStreamString(dbNodeId, 2);
    }
    else if(commandKey == 'd'){
      dbNodeId = message.substr(2);
      dbNodeId = tool::asStreamString(dbNodeId, 2);
    }
    rdt::RDTSocket queryConnection;
    std::vector<tool::IdNetNode> idNetNodeList;
    repoInfoMapMutex.lock();
    std::transform(repositoryConnectionPool.begin(), repositoryConnectionPool.end(), std::back_inserter(idNetNodeList), [](auto &key){ return key.first;});
    std::pair<std::pair<uint16_t, uint16_t>, std::string> info = repositoryConnectionPool[idNetNodeList[dbNodeId[0] % onlineRepositoriesCount]];
    repoInfoMapMutex.unlock();
    if(queryConnection.connect(GET_IP_ADDRESS(info), QUERY_PORT(info)) != net::Status::Done)
      return;
    if(queryConnection.online()){
      queryConnection.send(message);
      queryConnection.receive(message);//if is Read it is necessary
      /*
        set details of message
      */
      queryConnection.disconnectInitializer();
      socket->send(message);
    }
    socket->passiveDisconnect();
  }
}

void app::ServerMaster::connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket> socket){
  tool::IdNetNode repositoryId = REPOSITORY_LIMIT + socket->getSocketFileDescriptor();
  uint16_t remoteListenLinkPort, remoteListenQueryPort;
  std::string message;
  socket->receive(message);
  remoteListenLinkPort = std::stoi(message);
  socket->receive(message);
  remoteListenQueryPort = std::stoi(message);
  repoInfoMapMutex.lock();
  repositoryConnectionPool[repositoryId] = {{remoteListenQueryPort, remoteListenLinkPort}, socket->getRemoteIpAddress()};
  repoInfoMapMutex.unlock();

  while(socket->online()){
    socket->receive(message);
    uint8_t commandKey = message[0];
    if(commandKey == COMMAND_RENAME){
      repoInfoMapMutex.lock();
      std::pair<std::pair<uint16_t, uint16_t>, std::string> temp = repositoryConnectionPool[repositoryId];
      repositoryConnectionPool.erase(repositoryId);
      repositoryConnectionPool[(repositoryId = std::stoull(message.substr(1)))] = temp;
      repoInfoMapMutex.unlock();
    }
    else if(commandKey == COMMAND_LINK || commandKey == COMMAND_DETACH){
      message = message.substr(1);
      std::list<tool::IdNetNode> commandBody = tool::parseRepoActiveCommand(message);
      manageLinkDetachCommands(commandKey, commandBody, remoteListenQueryPort, remoteListenLinkPort, socket->getRemoteIpAddress(), repositoryId);
    }
    else if(commandKey == COMMAND_LIST){
      std::vector<std::pair<tool::IdNetNode, std::pair<std::pair<uint16_t, uint16_t>, std::string>>> graph;
      repoInfoMapMutex.lock();
      std::transform(repositoryConnectionPool.begin(), repositoryConnectionPool.end(), std::back_inserter(graph), [](auto &key){ return key;});
      repoInfoMapMutex.unlock();
      std::string graphStruct = "";
      if(!graph.empty()){
        for(auto& item : graph){
          std::string obtainedList;
          rdt::RDTSocket listRequestConnection;
          if(listRequestConnection.connect(GET_IP_ADDRESS(item.second), QUERY_PORT(item.second)) == net::Status::Done){
            if(listRequestConnection.online()){
              listRequestConnection.send("?");
              listRequestConnection.receive(obtainedList);
              obtainedList = std::to_string(item.first) + "*" + obtainedList;
              graphStruct += ("|" + obtainedList);
              listRequestConnection.disconnectInitializer();
            }
          }
        }
        graphStruct = graphStruct.substr(1);
        socket->send(graphStruct);
      }
      else{
        socket->send("empty");
      }
    }
    else if(commandKey == COMMAND_KILL){
      std::string obtainedList;
      rdt::RDTSocket listRequirementConnection;
      if(listRequirementConnection.connect(socket->getRemoteIpAddress(), remoteListenQueryPort) == net::Status::Done){
        listRequirementConnection.send("?");
        listRequirementConnection.receive(obtainedList);
        listRequirementConnection.disconnectInitializer();
        std::list<tool::IdNetNode> targets = tool::parseRepoActiveCommand(obtainedList);
        manageLinkDetachCommands(COMMAND_DETACH, targets, remoteListenQueryPort, remoteListenLinkPort, socket->getRemoteIpAddress(), repositoryId);
      }
      socket->disconnectInitializer();
      repoCountMutex.lock();
      --onlineRepositoriesCount;
      repoCountMutex.unlock();
    }
  }
}

void app::ServerMaster::manageLinkDetachCommands(const uint8_t& commandKey, std::list<tool::IdNetNode> commandBody, 
                                                  const uint16_t& port1, const uint16_t& port2, 
                                                  const std::string& remoteIp, tool::IdNetNode id){
  std::string prompt;
  prompt.push_back(commandKey);
  prompt += std::to_string(id);
  if(commandKey == COMMAND_LINK)
    prompt += "|" + std::to_string(port1) + "|" + std::to_string(port2) + "|" + remoteIp;
  for(auto& item : commandBody){
    std::shared_ptr<rdt::RDTSocket> linkConnection = std::make_shared<rdt::RDTSocket>();
    std::map<tool::IdNetNode, std::pair<std::pair<uint16_t, uint16_t>, std::string>>::iterator finder;
    repoInfoMapMutex.lock();
    finder = repositoryConnectionPool.find(item);
    if(finder == repositoryConnectionPool.end()){
      repoInfoMapMutex.unlock();
      alternateConsolePrintMutex.lock();
      tool::ConsolePrint("=>[LINK INTENT <Error>]: The given ID was not found.", RED);
      alternateConsolePrintMutex.unlock();
      continue;
    }
    repoInfoMapMutex.unlock();
    std::pair<std::pair<uint16_t, uint16_t>, std::string> dest = finder->second;
    if(linkConnection->connect(GET_IP_ADDRESS(dest), QUERY_PORT(dest)) == net::Status::Done){
      std::thread linkIntentThread(&ServerMaster::connEnvironmentLinkIntent, this, linkConnection, prompt);
      linkIntentThread.detach();
    }
  }
}

void app::ServerMaster::connEnvironmentLinkIntent(std::shared_ptr<rdt::RDTSocket> socket, const std::string& prompt){
  if(socket->online()){
    socket->send(prompt);
    socket->disconnectInitializer();
  }
}

void app::ServerMaster::runRepositoryListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newRepositoryIntent;
    newRepositoryIntent = std::make_shared<rdt::RDTSocket>();
    alternateConsolePrintMutex.lock();
    tool::ConsolePrint("=>[SERVER MASTER <Spam>]: Waiting for a new repository...", VIOLET);
    alternateConsolePrintMutex.unlock();
    if(repositoryListener.accept(*newRepositoryIntent) == net::Status::Done){
      repositoryConnectionPool[REPOSITORY_LIMIT + newRepositoryIntent->getSocketFileDescriptor()] = {{0, 0}, newRepositoryIntent->getRemoteIpAddress()};
      ++onlineRepositoriesCount;
      std::thread repositoryThread(&ServerMaster::connEnvironmentRepository, this, newRepositoryIntent);
      repositoryThread.detach();
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
    tool::ConsolePrint("=>[SERVER MASTER <Spam>]: Waiting for a new connection...", VIOLET);
    alternateConsolePrintMutex.unlock();
    if(clientListener.accept(*newClientIntent) == net::Status::Done){
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
