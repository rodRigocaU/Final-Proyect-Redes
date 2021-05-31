#ifndef CLIENT_0_MAIN_SERVER_PARSER_HPP_
#define CLIENT_0_MAIN_SERVER_PARSER_HPP_

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Tools/Fixer.hpp"
#include "../Tools/Colors.hpp"

namespace trlt
{

  struct CreateNodePacket
  {
    std::string nodeId;
    std::map<std::string, std::string> attributes;
    std::vector<std::string> relations;

    void clear(){
      nodeId = "";
      attributes.clear();
      relations.clear();
    }
  };

  CreateNodePacket& operator<<(CreateNodePacket& packet, const std::string& message)
  {
    packet.clear();
    if(message.length() && message[0] != 'c')
    {
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

  CreateNodePacket& operator>>(CreateNodePacket& packet, std::string& message)
  {
    message.clear();
    message += "c";
    std::string nameSize = std::to_string(packet.nodeId.length()), nAttrs, nRels;
    nAttrs = std::to_string(packet.attributes.size());
    nRels = std::to_string(packet.relations.size());
    message += tool::fixToBytes(nameSize, 3);
    message += packet.nodeId;
    message += tool::fixToBytes(nAttrs, 2);
    message += tool::fixToBytes(nRels, 3);

    for(auto& attr : packet.attributes)
    {
      std::string attNsize = std::to_string(attr.first.length());
      std::string attVsize = std::to_string(attr.second.length());
      message += tool::fixToBytes(attNsize, 3);
      message += attr.first;
      message += tool::fixToBytes(attVsize, 3);
      message += attr.second;
    }
    
    for(auto& rel : packet.relations)
    {
      std::string relNsize = std::to_string(rel.length());
      message += tool::fixToBytes(relNsize, 3);
      message += rel;
    }
    return packet;
  }

  struct ReadNodePacket {
    enum Class{Leaf, Internal, NoneClass};
    enum BooleanOperator{And, Or, NoneBO};
    enum QueryMode{Required, NotRequired, NoneQM};
    enum SqlOperator{Equal, LessThan, MoreThan, Like, NoneSQL};

    const std::unordered_map<SqlOperator, std::string> sqlOperators  = {{SqlOperator::Equal, "="},
                                                                        {SqlOperator::LessThan, "<"},
                                                                        {SqlOperator::MoreThan, ">"},
                                                                        {SqlOperator::Like, "like"}};

    struct Feature {
      std::string attrName, attrValue;
      SqlOperator sqlOpId = SqlOperator::NoneSQL;
      BooleanOperator boolOpId = BooleanOperator::NoneBO;
    };

    std::string nodeId;
    uint8_t depth;
    Class nodeType;
    QueryMode attribsReq;

    std::vector<Feature> features;

    ReadNodePacket() {
      clear();
    }
    
    void clear() {
      nodeId = "";
      attribsReq = QueryMode::NoneQM;
      nodeType = Class::NoneClass;
      features.clear();
    }
  };

  ReadNodePacket& operator<<(ReadNodePacket& packet, const std::string& message) {
    packet.clear();
    if(message.length() && message[0] != 'r') {
      tool::ConsolePrint("[Error]: Incorrect key header.", RED);
      exit(EXIT_FAILURE);
    }
    std::string content = message.substr(1);
    packet.nodeId = tool::asStreamString(content, 2);
    packet.depth = tool::asStreamNumeric(content, 1);
    packet.nodeType = static_cast<ReadNodePacket::Class>(tool::asStreamNumeric(content, 1));
    packet.attribsReq = static_cast<ReadNodePacket::QueryMode>(tool::asStreamNumeric(content, 1));
    std::size_t nFeatures = tool::asStreamNumeric(content, 2);
    while(nFeatures-- != 0) {
      ReadNodePacket::Feature feature;
      feature.attrName = tool::asStreamString(content, 3);
      feature.sqlOpId = static_cast<ReadNodePacket::SqlOperator>(tool::asStreamNumeric(content, 1));
      feature.attrValue = tool::asStreamString(content, 3);
      feature.boolOpId = static_cast<ReadNodePacket::BooleanOperator>(tool::asStreamNumeric(content, 1));
      packet.features.push_back(feature);
    }
    return packet;
  }

  ReadNodePacket& operator>>(ReadNodePacket& packet, std::string& message) {
    message.clear();
    message += "r";
    std::string nameSize = std::to_string(packet.nodeId.length());
    message += tool::fixToBytes(nameSize, 2);
    message += packet.nodeId;
    message += std::to_string(packet.depth);
    message += std::to_string(packet.nodeType);
    message += std::to_string(packet.attribsReq);
    message += tool::fixToBytes(std::to_string(packet.features.size()), 2);
    for(auto& feature : packet.features){
      message += std::to_string(feature.attrName.length());
      message += feature.attrName;
      message += std::to_string(feature.sqlOpId);
      message += std::to_string(feature.attrValue.length());
      message += feature.attrValue;
      message += std::to_string(feature.boolOpId);
    }
    return packet;
  }

  struct UpdateNodePacket {
    enum Mode{Object, Attribute, None};

    Mode updateMode;
    std::string nodeId, newNodeValue, attrName, attrValue;

    void clear(){
      nodeId = newNodeValue = attrName = attrValue = "";
      updateMode = Mode::None;
    }
  };

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

  struct DeleteNodePacket {
    enum Mode{Object, Attribute, Relation, None};

    Mode deleteMode;
    std::string nodeId, targetName, targetValue;

    void clear() {
      nodeId = targetName = targetValue = "";
      deleteMode = Mode::None;
    }
  };

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
    packet.targetValue = tool::asStreamString(content, 3);
    return packet;
  }

  DeleteNodePacket& operator>>(DeleteNodePacket& packet, std::string& message) {
    message.clear();
    message += "d";
    message += std::to_string(packet.deleteMode);
    message += tool::fixToBytes(std::to_string(packet.nodeId.length()), 2);
    message += tool::fixToBytes(std::to_string(packet.targetName.length()), 3);
    message += packet.targetName;
    message += tool::fixToBytes(std::to_string(packet.targetValue.length()), 3);
    message += packet.targetValue;
    return packet;
  }
}

#endif//CLIENT_0_MAIN_SERVER_PARSER_HPP_