#include "Network/RDTSocket.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "DataBase/Sqlite.hpp"
#include "Network/Algorithm/RDTEstimator.hpp"

int main() {
  //TESTING SETTINGS PARSER
  std::map<std::string, std::string> requirements = {{"DataBase",""},{"ServerMasterPort",""},{"RepositoryServerIp",""}};
  tool::readSettingsFile("Cenapse.conf", requirements, true);
  for(auto& item : requirements){
    std::cout << item.first << '=' << item.second << std::endl;
  }
  /*
    IDEA: 
      REPO -Conn-> MASTER
      MASTER -IpPort as Listener Port-> SAVE
      REPO -Change ID -> MASTER
      REPO -AskToConnectWithOtherRepos-> MASTER
      MASTER -Send ID Repo-> REPOSGROUP
      
  */

  /*
  rdt::RDTSocket slaveServerSocket;
  slaveServerSocket.setReceptorSettings("", 8000);
  std::string received_comand, remoteIp;
  uint16_t remotePort;

  db::SQLite BD;
  BD.createTables();
  
  slaveServerSocket.receive(received_comand, remoteIp, remotePort);
  std::cout << received_comand << " " << remoteIp << " " << remotePort << '\n';

  std::string nodeName;
  std::vector<std::pair<std::string, std::string>> attributes;

  msg::CreateNodePacket c;
  c << received_comand;
  std::cout << c.nodeId << '\n';

  for (auto atr : c.attributes)
    attributes.push_back(atr);
  


  BD.Create(c.nodeId,attributes,c.relations);
  BD.printSelectNodos();
  BD.printSelectAttributes();
  BD.printSelectRelations();
*/
  return 0;
}