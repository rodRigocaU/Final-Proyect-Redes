#include "App/Server/RepositoryInterface.hpp"
#include "App/Tools/Colors.hpp"
#include "Network/Algorithm/base64.hpp"
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
  std::string query;
  if(socket->online()){
    socket->receive(query);
    uint8_t commandKey = query[0];
    if(commandKey == 'c'){
      msg::CreateNodePacket qPacket;
      qPacket << query;
      for(auto& item : qPacket.attributes){
        if(item.second[0] == '@'){
          std::string decoded = crypto::decodeBase64(item.second.substr(1));
          decoded.pop_back();
          item.second = "@" + decoded;
        }
      }
      databaseMutex.lock();
      database.Create(qPacket);
      databaseMutex.unlock();
    }
    else if(commandKey == 'r'){
      std::string propagation = "", response = "";
      msg::ReadNodePacket qPacket;
      qPacket << query;
      databaseMutex.lock();
      auto readCopy = database.Read(qPacket);
      databaseMutex.unlock();
      for(auto& read : readCopy){
        propagation += read.first + ",";
        if(!read.second.empty()){
          response += "<" + read.first + ">";
          for(auto& ans : read.second)
            response += "[" + ans.first + ":" + ans.second + "],";
        }
      }
      if(!propagation.empty())
        propagation.pop_back();
      if(!response.empty())
        response.pop_back();
      else
        response = "empty";
      socket->send(propagation);
      socket->send(response);
      qPacket >> query;
      socket->send(query);
    }
    else if(commandKey == 'u'){
      msg::UpdateNodePacket qPacket;
      qPacket << query;
      databaseMutex.lock();
      database.Update(qPacket);
      databaseMutex.unlock();
    }
    else if(commandKey == 'd'){
      msg::DeleteNodePacket qPacket;
      qPacket << query;
      databaseMutex.lock();
      database.Delete(qPacket);
      databaseMutex.unlock();
    }
    alternateConsolePrintMutex.lock();
    tool::ConsolePrint("========================================", NO_COLOR);
    tool::ConsolePrint("[REPOSITORY <Spam>]: SELECT * NODOS_____", GREEN);
    database.printSelectNodos();
    tool::ConsolePrint("[REPOSITORY <Spam>]: SELECT * ATTRIBUTES", VIOLET);
    database.printSelectAttributes();
    tool::ConsolePrint("[REPOSITORY <Spam>]: SELECT * RELATIONS_", CYAN);
    database.printSelectRelations();
    tool::ConsolePrint("========================================", NO_COLOR);
    alternateConsolePrintMutex.unlock();
    socket->passiveDisconnect();
  }
}

void app::RepositoryServer::run(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newQueryIntent = std::make_shared<rdt::RDTSocket>();
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