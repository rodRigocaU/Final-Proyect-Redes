#ifndef CREATE_HPP_
#define CREATE_HPP_

#include "IncludeH.hpp"

namespace msg{

  struct CreateNodePacket {
    std::string nodeId;
    std::map<std::string, std::string> attributes;
    std::vector<std::string> relations;

    void clear();
  };

  CreateNodePacket& operator<<(CreateNodePacket& packet, const std::string& message);
  CreateNodePacket& operator>>(CreateNodePacket& packet, std::string& message);
  CreateNodePacket& operator<<(CreateNodePacket& packet, std::map<std::string, std::string>& settings);

}

#endif//CREATE_HPP_