#ifndef DELETE_HPP_
#define DELETE_HPP_

#include "IncludeH.hpp"

namespace msg{

  struct DeleteNodePacket {
    enum Mode{Object, Attribute, Relation, None};

    Mode deleteMode;
    std::string nodeId, targetName;

    void clear();
  };

  DeleteNodePacket& operator<<(DeleteNodePacket& packet, const std::string& message);
  DeleteNodePacket& operator>>(DeleteNodePacket& packet, std::string& message);
  DeleteNodePacket& operator<<(DeleteNodePacket& packet, std::map<std::string, std::string>& settings);

}

#endif//DELETE_HPP_