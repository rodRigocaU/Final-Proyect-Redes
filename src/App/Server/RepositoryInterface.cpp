#include "App/Server/RepositoryInterface.hpp"
#include "App/Tools/Colors.hpp"
#include <thread>
#include <sstream>

app::RepositoryServer::RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort, const std::string& databaseFile){
  tool::ConsolePrint("[REPOSITORY <Start protocol>]:", CYAN);
  if(masterServerSocket.connect(serverMasterIp, serverMasterPort) != net::Status::Done){
    tool::ConsolePrint("[Error]: Master isn\'t online.", RED);
    exit(EXIT_SUCCESS);
  }
  if(unknownQueryListener.listen(0) != net::Status::Done){
    tool::ConsolePrint("[Error]: Can\'t bind query listener.", RED);
    exit(EXIT_FAILURE);
  }
  masterServerSocket.send(std::to_string(unknownQueryListener.getLocalPort()));
  masterServerSocket.disconnectInitializer();
  tool::ConsolePrint("[REPOSITORY <Link Listener>]:       ONLINE", GREEN);
  tool::ConsolePrint("[REPOSITORY <Query Listener>]:      ONLINE", GREEN);
  database.setFile(databaseFile);
  tool::ConsolePrint("[REPOSITORY <Database Connection>]: ONLINE", GREEN);
  tool::ConsolePrint("=================================================", VIOLET);
}

app::RepositoryServer::~RepositoryServer(){
  unknownQueryListener.close();
}

void app::RepositoryServer::connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket> socket){
  std::string query, response;
  if(socket->online()){
    socket->receive(query);
    uint8_t commandKey = query[0];
    if(commandKey == 'c'){
      msg::CreateNodePacket qPacket;
      qPacket << query;
      database.Create(qPacket);
    }
    else if(commandKey == 'r'){
      msg::ReadNodePacket qPacket;
      qPacket << query;
      response = "";
      for(auto& neighbour : database.Read(qPacket)){
        response += neighbour + ",";
      }
      socket->send(response.substr(0, response.length() - 1));
      qPacket >> query;
      socket->send(query);
    }
    else if(commandKey == 'u'){
      msg::UpdateNodePacket qPacket;
      qPacket << query;
      database.Update(qPacket);
    }
    else if(commandKey == 'd'){
      msg::DeleteNodePacket qPacket;
      qPacket << query;
      database.Delete(qPacket);
    }
    socket->passiveDisconnect();
  }
}

void app::RepositoryServer::run(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newQueryIntent;
    newQueryIntent = std::make_shared<rdt::RDTSocket>();
    alternateConsolePrintMutex.lock();
    tool::ConsolePrint("[REPOSITORY <Spam>]: Waiting for a new Query.", CYAN);
    alternateConsolePrintMutex.unlock();
    if(unknownQueryListener.accept(*newQueryIntent) == net::Status::Done){
      std::thread queryThread(&RepositoryServer::connEnvironmentQuery, this, newQueryIntent);
      queryThread.detach();
      alternateConsolePrintMutex.lock();
      tool::ConsolePrint("[REPOSITORY <Spam>]: New query intent accepted.", GREEN);
      alternateConsolePrintMutex.unlock();
    }
  }
}