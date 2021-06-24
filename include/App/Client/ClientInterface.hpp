#ifndef CLIENT_INTERFACE_HPP_
#define CLIENT_INTERFACE_HPP_

#include "../../Network/RDTMask.hpp"

#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace app {

  class Client {
  private:
    std::unordered_map<std::string, std::function<bool(void)>> commands;

    bool create();
    bool read();
    bool update();
    bool drop();
    
    rdt::RDTSocket remoteSocket;
  public:
  Client(const std::string& serverIp, const std::string& serverPort);
  bool setCommand(const std::string& command);
  };
}

#endif //CLIENT_INTERFACE_HPP_