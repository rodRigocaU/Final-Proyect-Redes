#include "App/Server/RepositoryInterface.hpp"

int main() {
  std::map<std::string, std::string> requirements = {{"ServerMasterIp",""},{"ServerMasterSlavePort",""},{"DataBasePath",""}};
  if(!tool::readSettingsFile("Cenapse.conf", requirements, true)){
    tool::ConsolePrint("[Error]: Requirements missed.", RED);
    return EXIT_FAILURE;
  }
  std::string customizedDataBase;
  std::cout << "Create/Load Database file(write the name): ";
  std::cin >> customizedDataBase;
  std::ifstream file;
  file.open(customizedDataBase);
  if(file.is_open())
    file.close();
  else
    system((std::string("cp ") + requirements["DataBasePath"] + " " + customizedDataBase).c_str());
  app::RepositoryServer repository(requirements["ServerMasterIp"], std::stoi(requirements["ServerMasterSlavePort"]), customizedDataBase);

  repository.run();
  return 0;
}