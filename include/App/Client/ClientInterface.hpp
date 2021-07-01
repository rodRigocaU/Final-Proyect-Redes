#ifndef CLIENT_INTERFACE_HPP_
#define CLIENT_INTERFACE_HPP_

#include "../../Network/RDTSocket.hpp"
#include "ClientCodeInput.hpp"
#include "App/Tools/Colors.hpp"
#include "App/Tools/Fixer.hpp"
#include "App/Tools/InterfacePerformance.hpp"

#include <iostream>
#include <functional>
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
    
    rdt::RDTSocket clientSocket;
    std::string serverMasterIp;
    uint16_t serverMasterPort;
  public:
    Client(const std::string& serverIp, const std::string& serverPort);
    bool setCommand(const std::string& command);
  };
}

#endif //CLIENT_INTERFACE_HPP_