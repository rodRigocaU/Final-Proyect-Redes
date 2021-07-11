#include "App/Server/RepositoryInterface.hpp"
#include "App/Tools/Colors.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include <thread>
#include <sstream>

app::RepositoryServer::RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort){
  tool::ConsolePrint("[REPOSITORY <Start protocol>]:", CYAN);
  if(masterServerSocket.connect(serverMasterIp, serverMasterPort) != net::Status::Done){
    tool::ConsolePrint("[Error]: Master isn\'t online.", RED);
    exit(EXIT_SUCCESS);
  }
  if(unknownLinkListener.listen(0) != net::Status::Done){
    tool::ConsolePrint("[Error]: Can\'t bind link listener.", RED);
    exit(EXIT_FAILURE);
  }
  if(unknownQueryListener.listen(0) != net::Status::Done){
    tool::ConsolePrint("[Error]: Can\'t bind query listener.", RED);
    exit(EXIT_FAILURE);
  }
  masterServerSocket.send(std::to_string(unknownLinkListener.getLocalPort()));
  masterServerSocket.send(std::to_string(unknownQueryListener.getLocalPort()));
  ownId = 0;
  tool::ConsolePrint("[REPOSITORY <Link Listener>]:  ONLINE", GREEN);
  tool::ConsolePrint("[REPOSITORY <Query Listener>]: ONLINE", GREEN);
  tool::ConsolePrint("=================================================", VIOLET);
}

app::RepositoryServer::~RepositoryServer(){
  unknownLinkListener.close();
  unknownQueryListener.close();
}


void app::RepositoryServer::connEnvironmentLink(std::shared_ptr<rdt::RDTSocket> socket){
  std::string futureNeighbourInfo, strRemoteId, strQueryPort, strLinkPort, remoteNeighbourIp;
  if(socket->online()){
    socket->receive(futureNeighbourInfo);
    std::stringstream buffer;
    buffer << futureNeighbourInfo;
    std::getline(buffer, strRemoteId, COMMAND_SEPARATOR);
    std::getline(buffer, strQueryPort, COMMAND_SEPARATOR);
    std::getline(buffer, strLinkPort, COMMAND_SEPARATOR);
    std::getline(buffer, remoteNeighbourIp);
    neighboursMapMutex.lock();
    neighbours[std::stoull(strRemoteId)] = {{std::stoi(strQueryPort), std::stoi(strLinkPort)}, remoteNeighbourIp};
    neighboursMapMutex.unlock();
    socket->passiveDisconnect();
  }
}

void app::RepositoryServer::connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket> socket){
  std::string query, response;
  if(socket->online()){
    socket->receive(query);
    uint8_t commandKey = query[0];
    if(commandKey == 'c'){
      msg::CreateNodePacket qPacket;
      qPacket << query;
      //SQLite
      socket->send("ok");
    }
    else if(commandKey == 'r'){
      msg::ReadNodePacket qPacket;
      qPacket << query;
      //SQLite + PROPAGATION
      /*
        FOR(NEIGHBOUR IN NEIGHBOURS)
          CONNECT QUERY LISTENER AND SEND QUERY(DEPTH - 1) + PATH
      */
     socket->send("response");
    }
    else if(commandKey == 'u'){
      msg::UpdateNodePacket qPacket;
      qPacket << query;
      //SQLite
      socket->send("ok");
    }
    else if(commandKey == 'd'){
      msg::DeleteNodePacket qPacket;
      qPacket << query;
      //SQLite
      socket->send("ok");
    }
    else if(commandKey == COMMAND_LINK){
      query = query.substr(1);
      std::string remoteRepoId, strQueryPort, strLinkPort;
      std::stringstream buffer;
      buffer << query;
      std::getline(buffer, remoteRepoId, COMMAND_SEPARATOR);
      std::getline(buffer, strQueryPort, COMMAND_SEPARATOR);
      std::getline(buffer, strLinkPort, COMMAND_SEPARATOR);
      uint16_t remoteQueryPort = std::stoi(strQueryPort);
      uint16_t remoteLinkPort = std::stoi(strLinkPort);
      std::string remoteRepoIp;
      buffer >> remoteRepoIp;
      rdt::RDTSocket linkConnection;
      if(linkConnection.connect(remoteRepoIp, remoteLinkPort) == net::Status::Done){
        neighboursMapMutex.lock();
        neighbours[std::stoull(remoteRepoId)] = {{remoteQueryPort, remoteLinkPort}, remoteRepoIp};
        neighboursMapMutex.unlock();
        response = std::to_string(ownId) + "|";
        response += std::to_string(unknownQueryListener.getLocalPort()) + "|";
        response += std::to_string(unknownLinkListener.getLocalPort()) + "|";
        response += unknownLinkListener.getLocalIp();
        linkConnection.send(response);
        linkConnection.disconnectInitializer();
      }
    }
    else if(commandKey == COMMAND_DETACH){
      query = query.substr(1);
      neighboursMapMutex.lock();
      neighbours.erase(std::stoull(query));
      neighboursMapMutex.unlock();
    }
    else if(commandKey == COMMAND_LIST){
      response = "";
      neighboursMapMutex.lock();
      for(auto& repoInfo : neighbours){
        response += std::to_string(repoInfo.first) + "&";
      }
      socket->send(response);
      neighboursMapMutex.unlock();
    }
    socket->passiveDisconnect();
  }
}

void app::RepositoryServer::runLinkListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newLinkIntent;
    newLinkIntent = std::make_shared<rdt::RDTSocket>();
    if(unknownLinkListener.accept(*newLinkIntent) == net::Status::Done){
      std::thread linkIntentThread(&RepositoryServer::connEnvironmentLink, this, newLinkIntent);
      linkIntentThread.detach();
    }
  }
}
    
void app::RepositoryServer::runQueryListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newQueryIntent;
    newQueryIntent = std::make_shared<rdt::RDTSocket>();
    if(unknownQueryListener.accept(*newQueryIntent) == net::Status::Done){
      std::thread queryThread(&RepositoryServer::connEnvironmentQuery, this, newQueryIntent);
      queryThread.detach();
    }
  }
}

void app::RepositoryServer::detachNeighbours(const std::string& nList){
  std::string nodeId;
  std::stringstream buffer;
  buffer << nList;
  while(std::getline(buffer, nodeId, COMMAND_SPLIT)){
    neighboursMapMutex.lock();
    neighbours.erase(std::stoull(nodeId));
    neighboursMapMutex.unlock();
  }
}

void app::RepositoryServer::run(){
  std::thread linkListenThread(&RepositoryServer::runLinkListener, this);
  std::thread queryListenThread(&RepositoryServer::runQueryListener, this);
  std::string message, body;
  int8_t commKey;
  while(masterServerSocket.online()){
    message.clear();
    std::cout << ">> ";
    std::cin >> commKey;
    switch(commKey){
      case COMMAND_LINK:
        message.push_back(commKey);
        std::cin >> body;
        message += body;
        masterServerSocket.send(message);
        break;
      case COMMAND_DETACH:  
        message.push_back(commKey);
        std::cin >> body;
        message += body;
        detachNeighbours(body);
        masterServerSocket.send(message);
        break;
      case COMMAND_LIST:
        message.push_back(commKey);
        masterServerSocket.send(message);
        masterServerSocket.receive(message);
        break;
      case COMMAND_RENAME:
        message.push_back(commKey);
        std::cout << "New Id(number): ";
        std::cin >> body;
        message += body;
        masterServerSocket.send(message);
        break;
      case COMMAND_KILL:
        message.push_back(commKey);
        masterServerSocket.send(message);
        masterServerSocket.passiveDisconnect();
        break;
    }
  }
  linkListenThread.join();
  queryListenThread.join();
}