#include "Network/RDTSocket.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "DataBase/Sqlite.hpp"
#include "Network/Algorithm/RDTEstimator.hpp"

int main() {
  //TESTING ESTIMATOR
  rdt::RTTEstimator ewmaEstimator;
  std::cout << ewmaEstimator.estimate() << std::endl;
  for(int i = 0; i < 20; ++i){
    int val = rand() % 500 + 1;
    std::cout << "Current time out: " << val << std::endl;
    std::cout << "EWMA time out: " << ewmaEstimator.estimate(val) << std::endl;
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

  // db::SQLite BD;
  // BD.createTables();
  
  slaveServerSocket.receive(received_comand, remoteIp, remotePort);
  std::cout << received_comand << " " << remoteIp << " " << remotePort << '\n';

  std::string nodeName;
  std::vector<std::pair<std::string, std::string>> attributes;

  // msg::CreateNodePacket c;
  // c << received_comand;
  // std::cout << c.nodeId << '\n';

  // for (auto atr : c.attributes)
  //   attributes.push_back(atr);
  


  BD.Create(c.nodeId,attributes,c.relations);
  BD.printSelectNodos();
  BD.printSelectAttributes();
  BD.printSelectRelations();
*/
  return 0;
}