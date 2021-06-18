#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <thread>

namespace app
{

  class MainServerApp {
    private:
    int port;
    std::string ip;
    struct sockaddr_in stSockAddr;
    int connection;
    int server;
    int socketServer;
    int socketCliente;
    int socketRepository; // se usa en el servidor respositorio
    char mode;
    int numberRepositories;
    int repository;
    
    int mainPort;
    std::string mainIp;

 
    std::map<int,std::string>   connections;
    std::vector<int> socketRepositories; // se usa en el maestro

    void runMainServer();
    void connMasterRepository(int pPort, string pIp);

  public:
    void runRepository();
    
    void setMainIp(string ip){mainIp = ip;};
    void setMainPort(int port){mainPort = port;};
    void setMode(char Pmode='S') { mode = Pmode;}
    void setPort(int Pport=50000) { port = Pport;}
    void setIp(string Pip="127.0.0.1") { ip = Pip;}
    void setNumberRepositories(int r){numberRepositories = r;}
    void registerRepository();   
  };

  void MainServerApp::runMainServer()
  {
    for(;;){
      int newCo.
      nnection = accept(socketServer, NULL, NULL);
      if(0 > newConnection){
        perror("error accept failed");
        close(newConnection);
        exit(EXIT_FAILURE);
      }
    //std::thread (&DGDB::runConnection,this,newConnection).detach();
    }
  }
 

}




#endif//SERVER_INTERFACE_HPP_