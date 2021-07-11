#include "App/Server/ServerInterface.hpp"

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
