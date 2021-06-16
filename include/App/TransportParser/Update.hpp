#ifndef UPDATE_HPP_
#define UPDATE_HPP_

#include "IncludeH.hpp"

namespace msg{

  struct UpdateNodePacket {
    enum Mode{Object, Attribute, None};

    Mode updateMode;
    std::string nodeId, newNodeValue, attrName, attrValue;

    void clear();
  };

  UpdateNodePacket& operator<<(UpdateNodePacket& packet, const std::string& message);
  UpdateNodePacket& operator>>(UpdateNodePacket& packet, std::string& message);
  UpdateNodePacket& operator<<(UpdateNodePacket& packet, std::map<std::string, std::string>& settings);

}

#endif//UPDATE_HPP_