#include "Network/UdpSocket.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "database/Sqlite.hpp"

#define PORT 8000

int main()
{
  net::UdpSocket slaveServerSocket("", "8000");
  std::string received_comand, IP_from;
  uint16_t Port_from;

  // db::SQLite BD;
  // BD.createTables();
  
  // slaveServerSocket.receive(received_comand, IP_from, Port_from);
  // std::cout << received_comand << " " << IP_from << " " << Port_from << '\n';

  // std::string name_nodo;
  // std::vector<std::pair<std::string, std::string>> attributes;

  // msg::CreateNodePacket c;
  // c << received_comand;
  // std::cout << c.nodeId << '\n';

  // for (auto atr : c.attributes)
  // {
  //   attributes.push_back(atr);
  // }


  // BD.Create(c.nodeId,attributes,c.relations);
  // BD.printSelectNodos();
  // BD.printSelectAttributes();
  // BD.printSelectRelations();


  
}