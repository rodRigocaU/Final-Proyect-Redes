#include "App/TransportParser/Read.hpp"

namespace msg{

  void ReadNodePacket::Feature::reset(){
    attrName = attrValue = "";
    sqlOpId = SqlOperator::NoneSQL;
    boolOpId = BooleanOperator::NoneBO;
  }

  ReadNodePacket::ReadNodePacket(){
    clear();
  }

  ReadNodePacket::BooleanOperator ReadNodePacket::toBooleanEnum(const std::string& boolOp){
    if(boolOp == "and")
      return BooleanOperator::And;
    else if(boolOp == "or")
      return BooleanOperator::Or;
    return BooleanOperator::NoneBO;
  }
      
  void ReadNodePacket::clear() {
    nodeId = "";
    attribsReq = QueryMode::NoneQM;
    nodeType = Class::NoneClass;
    features.clear();
  }

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
      message += tool::fixToBytes(std::to_string(feature.attrName.length()), 3);
      message += feature.attrName;
      message += std::to_string(feature.sqlOpId);
      message += tool::fixToBytes(std::to_string(feature.attrValue.length()), 3);
      message += feature.attrValue;
      message += std::to_string(feature.boolOpId);
    }
    return packet;
  }

  ReadNodePacket& operator<<(ReadNodePacket& packet, std::map<std::string, std::string>& settings){
    packet.clear();
    packet.nodeId = settings[CENAPSE_CODE_NODE_NAME];
    packet.depth = std::stoi(settings[CENAPSE_CODE_DEPTH]);
    std::string readType = settings[CENAPSE_CODE_LEAF];
    if(readType == CENAPSE_CODE_BOOLEAN_TRUE)
      packet.nodeType = ReadNodePacket::Class::Leaf;
    else if(readType == CENAPSE_CODE_BOOLEAN_FALSE)
      packet.nodeType = ReadNodePacket::Class::Internal;
    else
      packet.nodeType = ReadNodePacket::Class::NoneClass;

    std::string requirements = settings[CENAPSE_CODE_ATRIBUTES_REQUIRED];

    if(requirements == CENAPSE_CODE_BOOLEAN_TRUE)
      packet.attribsReq = ReadNodePacket::QueryMode::Required;
    else if(requirements == CENAPSE_CODE_BOOLEAN_FALSE)
      packet.attribsReq = ReadNodePacket::QueryMode::NotRequired;
    else
      packet.attribsReq = ReadNodePacket::QueryMode::NoneQM;
    std::string rawFeatureComponent;
    std::stringstream featureGroup;
    featureGroup << settings[CENAPSE_CODE_QUERY_FEATURES];
    ReadNodePacket::Feature singleFeature;
    uint8_t state = 0;
    while(std::getline(featureGroup, rawFeatureComponent, '|')){
      tool::cleanSpaces(rawFeatureComponent);
      std::cout << rawFeatureComponent << std::endl;
      if(state == 0){
        singleFeature.attrName = rawFeatureComponent;
      }
      else if(state == 1){
        singleFeature.sqlOpId = packet.sqlMeanings.find(rawFeatureComponent)->second;
      }
      else if(state == 2){
        singleFeature.attrValue = rawFeatureComponent;
      }
      else{
        singleFeature.boolOpId = packet.toBooleanEnum(rawFeatureComponent);
        packet.features.push_back(singleFeature);
        singleFeature.reset();
        state = 0;
        continue;
      }
      ++state;
    }
    if(state != 0)
      packet.features.push_back(singleFeature);
    std::cout << state << std::endl;
    return packet;
  }

}