#include "Network/RDTSocket.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "App/Server/RepositoryInterface.hpp"

int main() {
  //TESTING SETTINGS PARSER
  std::map<std::string, std::string> requirements = {{"DataBase",""},{"ServerMasterPort",""},{"RepositoryServerIp",""}};
  tool::readSettingsFile("Cenapse.conf", requirements, true);
  for(auto& item : requirements){
    std::cout << item.first << '=' << item.second << std::endl;
  }
  

  return 0;
}