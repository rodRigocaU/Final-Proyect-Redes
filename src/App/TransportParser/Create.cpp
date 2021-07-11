#include "App/TransportParser/Create.hpp"

namespace msg{

  void CreateNodePacket::clear(){
    nodeId = "";
    attributes.clear();
    relations.clear();
  }

  CreateNodePacket& operator<<(CreateNodePacket& packet, const std::string& message){
    packet.clear();
    if(message.length() && message[0] != 'c'){
      tool::ConsolePrint("[Error]: Incorrect key header.", RED);
      exit(EXIT_FAILURE);
    }
    std::string content = message.substr(1);
    packet.nodeId = tool::asStreamString(content, 3);
    std::size_t nAttrs = tool::asStreamNumeric(content, 2);
    std::size_t nRels = tool::asStreamNumeric(content, 3);

    while(nAttrs-- != 0){
      std::string name = tool::asStreamString(content, 3);
      std::string value = tool::asStreamString(content, 3);
      packet.attributes[name] = value;
    }
    while(nRels-- != 0)
      packet.relations.push_back(tool::asStreamString(content, 3));
    return packet;
  }

  CreateNodePacket& operator>>(CreateNodePacket& packet, std::string& message){
    message.clear();
    message += "c";
    std::string nameSize = std::to_string(packet.nodeId.length()), nAttrs, nRels;
    nAttrs = std::to_string(packet.attributes.size());
    nRels = std::to_string(packet.relations.size());
    message += tool::fixToBytes(nameSize, 3);
    message += packet.nodeId;
    message += tool::fixToBytes(nAttrs, 2);
    message += tool::fixToBytes(nRels, 3);

    for(auto& attr : packet.attributes){
      std::string attNsize = std::to_string(attr.first.length());
      std::string attVsize = std::to_string(attr.second.length());
      message += tool::fixToBytes(attNsize, 3);
      message += attr.first;
      message += tool::fixToBytes(attVsize, 3);
      message += attr.second;
    }
    
    for(auto& rel : packet.relations){
      std::string relNsize = std::to_string(rel.length());
      message += tool::fixToBytes(relNsize, 3);
      message += rel;
    }
    return packet;
  }

  CreateNodePacket& operator<<(CreateNodePacket& packet, std::map<std::string, std::string>& settings){
    packet.clear();
    packet.nodeId = settings[CENAPSE_CODE_NODE_NAME];
    std::stringstream attributeGroup, relationGroup, buffer;
    attributeGroup << settings[CENAPSE_CODE_ATRIBUTES];
    relationGroup << settings[CENAPSE_CODE_RELATIONS];
    std::string singleAttribute, singleRelation;
    while(std::getline(attributeGroup, singleAttribute, ',')){
      std::string key, value;
      buffer << singleAttribute;
      std::getline(buffer, key, '|');
      std::getline(buffer, value);
      tool::cleanSpaces(key);
      tool::cleanSpaces(value);
      packet.attributes.insert({key, value});
      buffer.clear();
    }
    while(std::getline(relationGroup, singleRelation, ',')){
      tool::cleanSpaces(singleRelation);
      packet.relations.push_back(singleRelation);
    }
    return packet;
  }

}