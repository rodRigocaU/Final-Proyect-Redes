#include "App/Client/ClientInterface.hpp"

int main(int argc, char *argv[]) {
  std::map<std::string, std::string> requirements = {{"ServerMasterIp",""},{"ServerMasterClientPort",""}};
  if(!tool::readSettingsFile("Cenapse.conf", requirements, true)) {
    tool::ConsolePrint("[Error]: Requirements missed.", RED);
    return EXIT_FAILURE;
  }
  app::Client clientInstance(requirements["ServerMasterIp"], requirements["ServerMasterClientPort"]);
  
  if(argc > 1) {
    if(!clientInstance.setCommand(argv[1])) {
      tool::ConsolePrint("[Error]: Can not process this command.", RED);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

