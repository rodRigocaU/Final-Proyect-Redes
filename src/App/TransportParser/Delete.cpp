#include "App/TransportParser/Delete.hpp"

using namespace msg;

void DeleteNodePacket::clear() {
  nodeId = targetName = "";
  deleteMode = Mode::None;
}

DeleteNodePacket& operator<<(DeleteNodePacket& packet, const std::string& message) {
  packet.clear();
  if(message.length() && message[0] != 'd') {
    tool::ConsolePrint("[Error]: Incorrect key header.", RED);
    exit(EXIT_FAILURE);
  }
  std::string content = message.substr(1);
  packet.deleteMode = static_cast<DeleteNodePacket::Mode>(tool::asStreamNumeric(content, 1));
  packet.nodeId = tool::asStreamString(content, 2);
  packet.targetName = tool::asStreamString(content, 3);
  return packet;
}

DeleteNodePacket& operator>>(DeleteNodePacket& packet, std::string& message) {
  message.clear();
  message += "d";
  message += std::to_string(packet.deleteMode);
  message += tool::fixToBytes(std::to_string(packet.nodeId.length()), 2);
  message += tool::fixToBytes(std::to_string(packet.targetName.length()), 3);
  message += packet.targetName;
  return packet;
}

DeleteNodePacket& operator<<(DeleteNodePacket& packet, std::map<std::string, std::string>& settings){
  packet.clear();
  packet.nodeId = settings["Node_Name"];
  std::string dropType = settings["Mode"];
  if(dropType == "node")
    packet.deleteMode = DeleteNodePacket::Mode::Object;
  else if(dropType == "attribute")
    packet.deleteMode = DeleteNodePacket::Mode::Attribute;
  else if(dropType == "relation")
    packet.deleteMode = DeleteNodePacket::Mode::Relation;
  else
    packet.deleteMode = DeleteNodePacket::Mode::None;
  packet.targetName = settings["Attribute/Relation"];
  return packet;
}