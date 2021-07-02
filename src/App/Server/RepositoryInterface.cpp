#include "App/Server/RepositoryInterface.hpp"
#include "App/Tools/Colors.hpp"
#include <thread>

app::RepositoryServer::RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort){
  if(masterServerSocket.connect(serverMasterIp, serverMasterPort) != net::Status::Done){
    tool::ConsolePrint("[Error]: Master isn\'t online.", RED);
    exit(EXIT_SUCCESS);
  }
}

app::RepositoryServer::~RepositoryServer(){

}

void app::RepositoryServer::run(){

}