#include "App/Server/ServerInterface.hpp"
#include <thread>

void recv_messages(rdt::RDTSocket& curr_socket)
{
  std::string msg;
  while(true){
    curr_socket.receive(msg);
    std::cout << "message: " << msg << "\n";
    std::cout << "Para: " << curr_socket << std::endl;
  }
}


int main(){
  std::map<std::string, std::string> requirements = {{"ServerMasterClientPort",""},{"ServerMasterSlavePort",""}};
  if(!tool::readSettingsFile("Cenapse.conf", requirements, true)){
    tool::ConsolePrint("[Error]: Requirements missed.", RED);
    return EXIT_FAILURE;
  }
  app::ServerMaster master(std::stoi(requirements["ServerMasterClientPort"]), std::stoi(requirements["ServerMasterSlavePort"]));

  master.run();
  return 0;
}
