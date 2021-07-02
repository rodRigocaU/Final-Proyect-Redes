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

  //*Probar la BD
  //C
  /*
  std::string received_comand = "c005Julio02001004EDAD00233009profesion010pprofesorr004Nodo";
  msg::CreateNodePacket c;
  c << received_comand;
  */

  //R (Incompleto)
  /*
  
  // std::string received_comand = "r05Julio101102004EDAD0002330009profesion3008profesor0"; //True
  // std::string received_comand = "r05Julio101402004EDAD0002340009profesion3008profesor0";
  std::string received_comand = "r05Julio110100";
  msg::ReadNodePacket r;
  r << received_comand;
  BD.Read(r.nodeId,r.depth,r.nodeType,r.attribsReq,r.features);
  */

  //U
  /*
  // std::string received_comandU = "U106Carlos06Karlos";  // ? Update only Nodo
  std::string received_comandU = "U006Carlos009apellidos005Grace ";  // ? Update only Nodo
  msg::UpdateNodePacket U;
  U << received_comandU;
  if (U.updateMode == msg::UpdateNodePacket::Mode::Object)
    BD.UpdateValueNodo(U.nodeId,U.newNodeValue);
  else
    BD.UpdateAttribute(U.nodeId,U.attrName,U.attrValue);
  */

  //D
  /*
  std::string received_comandD = "D005Julio";  // ? Eliminación de nodo
  // std::string received_comandD = "D105Julio015GradoAcademicos ";  // ? Eliminación de un atributo del nodo
  // std::string received_comandD = "D205Julio017RelacionAcademico ";  // ? Eliminación de una relación del nodo:
  msg::DeleteNodePacket D;
  D << received_comandD;
  if (D.deleteMode == msg::DeleteNodePacket::Mode::Object)
    BD.DropNode(D.nodeId);
  else if (D.deleteMode == msg::DeleteNodePacket::Mode::Attribute)
    BD.DropValueAttribute(D.nodeId,D.targetName);
  else //*Relation
    BD.DropRelation(D.nodeId,D.targetName);
  */

  return 0;
}