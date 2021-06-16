#include "App/TransportParser/Update.hpp"

using namespace msg;

void UpdateNodePacket::clear(){
  nodeId = newNodeValue = attrName = attrValue = "";
  updateMode = Mode::None;
}

UpdateNodePacket& operator<<(UpdateNodePacket& packet, const std::string& message) {
  packet.clear();
  if(message.length() && message[0] != 'u'){
    tool::ConsolePrint("[Error]: Incorrect key header.", RED);
    exit(EXIT_FAILURE);
  }
  std::string content = message.substr(1);
  packet.updateMode = static_cast<UpdateNodePacket::Mode>(tool::asStreamNumeric(content, 1));
  packet.nodeId = tool::asStreamString(content, 2);
  packet.newNodeValue = tool::asStreamString(content, 2);
  packet.attrName = tool::asStreamString(content, 3);
  packet.attrValue = tool::asStreamString(content, 3);
  return packet;
}

UpdateNodePacket& operator>>(UpdateNodePacket& packet, std::string& message) {
  message.clear();
  message += "u";
  message += std::to_string(packet.updateMode);
  message += tool::fixToBytes(std::to_string(packet.nodeId.length()), 2);
  message += packet.nodeId;
  message += tool::fixToBytes(std::to_string(packet.newNodeValue.length()), 2);
  message += packet.newNodeValue;
  message += tool::fixToBytes(std::to_string(packet.attrName.length()), 3);
  message += packet.attrName;
  message += tool::fixToBytes(std::to_string(packet.attrValue.length()), 3);
  message += packet.attrValue;
  return packet;
}

UpdateNodePacket& operator<<(UpdateNodePacket& packet, std::map<std::string, std::string>& settings){
  packet.clear();
  packet.nodeId = settings["Node_Name"];
  std::string updateType = settings["Mode"];
  if(updateType == "node")
    packet.updateMode = UpdateNodePacket::Mode::Object;
  else if(updateType == "attribute")
    packet.updateMode = UpdateNodePacket::Mode::Attribute;
  else
    packet.updateMode = UpdateNodePacket::Mode::None;
  packet.newNodeValue = settings["Node_Value"];
  std::stringstream attributeGroup;
  attributeGroup << settings["Attribute"];
  std::string nameAttr, valAttr;
  std::getline(attributeGroup, nameAttr, '|');
  std::getline(attributeGroup, valAttr);
  tool::cleanSpaces(nameAttr);
  tool::cleanSpaces(valAttr);
  packet.attrName = nameAttr;
  packet.attrValue = valAttr;
  return packet;
}
