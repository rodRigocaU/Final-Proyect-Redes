#include "Network/Socket.hpp"
// #include "database/Sqlite.hpp"

#define PORT 8000

int main(){
  RDT::UdpSocket slaveServerSocket("", "8000");
  std::string received_comand, IP_from;
  uint16_t Port_from;

  slaveServerSocket.receive(received_comand, IP_from, Port_from);
  //NOMBRE DE NODO
  //ATRIBUTOS 
  //RELACIONES
  // el comando de creación se guardará en el string received_comand

  // database::SQLite BD;
  // BD.createTables();

  
  // string name_nodo;
  // vector<pair<string,string> >attributes;
  // vector<string> nodes_relations;
  
  
  // BD.Create(name_nodo,attributes,nodes_relations);

  return 0;
}