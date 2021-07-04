#include "App/Server/RepositoryInterface.hpp"
#include "App/Tools/Colors.hpp"
#include <thread>

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
  tool::ConsolePrint("=================================================", VIOLET);
}

app::RepositoryServer::~RepositoryServer(){
  unknownLinkListener.close();
  unknownQueryListener.close();
}

void app::RepositoryServer::connEnvironmentLink(std::shared_ptr<rdt::RDTSocket>& socket){

}

void app::RepositoryServer::connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket>& socket){

}

void app::RepositoryServer::runLinkListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newLinkIntent;
    newLinkIntent = std::make_shared<rdt::RDTSocket>();
    alternateConsolePrintMutex.lock();
    std::cout << "=>[REPOSITORY <Spam>]: Waiting for a new link intent..." << std::endl;
    alternateConsolePrintMutex.unlock();
    if(unknownLinkListener.accept(*newLinkIntent) == net::Status::Done){
      neighbourConnectionPool[newLinkIntent->getSocketFileDescriptor()] = newLinkIntent;
      std::thread clientThread(&RepositoryServer::connEnvironmentLink, this, newLinkIntent);
      clientThread.detach();
      alternateConsolePrintMutex.lock();
      tool::ConsolePrint("=>[REPOSITORY <Spam>]: Link intent accepted.", CYAN);
      std::cout << *newLinkIntent << std::endl;
      alternateConsolePrintMutex.unlock();
    }
  }
}
    
void app::RepositoryServer::runQueryListener(){
  while(true){
    std::shared_ptr<rdt::RDTSocket> newQueryIntent;
    newQueryIntent = std::make_shared<rdt::RDTSocket>();
    alternateConsolePrintMutex.lock();
    std::cout << "=>[REPOSITORY <Spam>]: Waiting for a new query..." << std::endl;
    alternateConsolePrintMutex.unlock();
    if(unknownQueryListener.accept(*newQueryIntent) == net::Status::Done){
      queryConnectionPool[newQueryIntent->getSocketFileDescriptor()] = newQueryIntent;
      std::thread clientThread(&RepositoryServer::connEnvironmentQuery, this, newQueryIntent);
      clientThread.detach();
      alternateConsolePrintMutex.lock();
      tool::ConsolePrint("=>[REPOSITORY <Spam>]: Accepted query and ready to comply.", CYAN);
      std::cout << *newQueryIntent << std::endl;
      alternateConsolePrintMutex.unlock();
    }
  }
}

void app::RepositoryServer::run(){
  std::thread linkListenThread(&RepositoryServer::runLinkListener, this);
  std::thread queryListenThread(&RepositoryServer::runQueryListener, this);
  std::string message;
  while(masterServerSocket.online()){
    std::cout << ">> ";
    std::cin >> message;
    masterServerSocket.send(message);
    /*
      set details of message
    */
  }
  linkListenThread.join();
  queryListenThread.join();
}