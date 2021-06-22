#include "Network/RDTMask.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "DataBase/Sqlite.hpp"

int main()
{
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
  


  // BD.Create(c.nodeId,attributes,c.relations);
  // BD.printSelectNodos();
  // BD.printSelectAttributes();
  // BD.printSelectRelations();

}