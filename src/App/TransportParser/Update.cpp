#include "App/TransportParser/Update.hpp"

namespace msg{

  void UpdateNodePacket::clear(){
    nodeId = newNodeValue = attrName = attrValue = "";
    updateMode = Mode::None;
  }

  UpdateNodePacket &operator<<(UpdateNodePacket &packet, const std::string &message){
    packet.clear();
    if (message.length() && message[0] != 'u'){
      tool::ConsolePrint("[Error]: Incorrect key header.", RED);
      exit(EXIT_FAILURE);
    }
    std::string content = message.substr(1);
    packet.updateMode = static_cast<UpdateNodePacket::Mode>(tool::asStreamNumeric(content, 1));
    packet.nodeId = tool::asStreamString(content, 2);

    if (packet.updateMode == UpdateNodePacket::Mode::Attribute){
      packet.attrName = tool::asStreamString(content, 3);
      packet.attrValue = tool::asStreamString(content, ATTRIBUTE_LENGTH);
    }
    else if (packet.updateMode == UpdateNodePacket::Mode::Object){
      packet.newNodeValue = tool::asStreamString(content, 2);
    }
    return packet;
  }

  UpdateNodePacket &operator>>(UpdateNodePacket &packet, std::string &message){
    message.clear();
    message += "u";
    message += std::to_string(packet.updateMode);
    message += tool::fixToBytes(std::to_string(packet.nodeId.length()), 2);
    message += packet.nodeId;
    if(packet.updateMode == UpdateNodePacket::Mode::Attribute){
      message += tool::fixToBytes(std::to_string(packet.attrName.length()), 3);
      message += packet.attrName;
      message += tool::fixToBytes(std::to_string(packet.attrValue.length()), ATTRIBUTE_LENGTH);
      message += packet.attrValue;
    }
    else if (packet.updateMode == UpdateNodePacket::Mode::Object){
      message += tool::fixToBytes(std::to_string(packet.newNodeValue.length()), 2);
      message += packet.newNodeValue;
    }
    return packet;
  }

  UpdateNodePacket &operator<<(UpdateNodePacket &packet, std::map<std::string, std::string> &settings){
    packet.clear();
    packet.nodeId = settings[CENAPSE_CODE_NODE_NAME];
    std::string updateType = settings[CENAPSE_CODE_NA_MODE];
    if (updateType == CENAPSE_CODE_NODE_OP){
      packet.updateMode = UpdateNodePacket::Mode::Object;
      packet.newNodeValue = settings[CENAPSE_CODE_NODE_VALUE];
    }
    else if (updateType == CENAPSE_CODE_ATRIBUTE_OP){
      packet.updateMode = UpdateNodePacket::Mode::Attribute;
      std::stringstream attributeGroup;
      attributeGroup << settings[CENAPSE_CODE_ATRIBUTE_I_VALUE];
      std::string nameAttr, valAttr;
      std::getline(attributeGroup, nameAttr, '|');
      std::getline(attributeGroup, valAttr);
      tool::cleanSpaces(nameAttr);
      tool::cleanSpaces(valAttr);
      packet.attrName = nameAttr;
      packet.attrValue = valAttr;
    }
    else
      packet.updateMode = UpdateNodePacket::Mode::None;
    return packet;
  }

}