#include "App/Server/ServerInterface.hpp"

namespace app{
  
  MainServer::MainServer(const uint16_t& localPort){
    //listener.setReceptorSettings("", localPort);
  }

  void MainServer::run(){
    std::string message, remoteIp;
    uint16_t remotePort;
    while(true){
      //listener.receive(message, remoteIp, remotePort);
    }
  }

}