#ifndef CLIENT_INTERFACE_HPP_
#define CLIENT_INTERFACE_HPP_

#include "App/Tools/Colors.hpp"
#include "App/Tools/Fixer.hpp"
#include "App/Tools/InterfacePerformance.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "Network/UdpSocket.hpp"

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
    
    std::unique_ptr<net::UdpSocket> remoteSocket;
  public:
  Client(const std::string& serverIp, const std::string& serverPort);
  bool setCommand(const std::string& command);
  };
}

#endif //CLIENT_INTERFACE_HPP_