#ifndef CLIENT_PROTOCOL_HANDLER_HPP_
#define CLIENT_PROTOCOL_HANDLER_HPP_

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "App/Tools/Colors.hpp"
#include "App/Tools/Fixer.hpp"
#include "App/Tools/InterfacePerformance.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"

namespace app {

  class Client {
  private:
    std::unordered_map<std::string, std::function<bool(void)>> commands;

    bool create();
    bool read();
    bool update();
    bool drop();

  public:
  Client(const std::string& serverIp, const std::string& remotePort);
  bool setCommand(const std::string& command);
  };

  Client::Client(const std::string& serverIp, const std::string& remotePort) {
    commands["spawn"]  = std::bind(&Client::create, this);
    commands["ask"]    = std::bind(&Client::read, this);
    commands["update"] = std::bind(&Client::update, this);
    commands["drop"]   = std::bind(&Client::drop, this);
  }

  bool Client::setCommand(const std::string& command) {
    std::unordered_map<std::string, std::function<bool(void)>>::iterator function = commands.find(command);
    if(function != commands.end())
      return function->second();
    return false;
  }

  bool Client::create() {
    trlt::CreateNodePacket packet;
    system("nano spawn.conf");
    std::map<std::string, std::string> settings;
    settings["Node_Name"] = "";
    settings["Attributes"] = "";
    settings["Relations"] = "";
    if(tool::readSettingsFile("spawn.conf", settings)){

    }
    return true;
  }

  bool Client::read() {
    trlt::ReadNodePacket packet;
    system("nano ask.conf");
    std::map<std::string, std::string> settings;
    settings["Node_Name"] = "";
    settings["Depth"] = "";
    settings["Leaf"] = "";
    settings["Query_Features"] = "";
    if(tool::readSettingsFile("ask.conf", settings)){
      for(auto& data : settings){
        std::cout << data.first << "->" << data.second << std::endl;
      }
    }
    return true;
  }

  bool Client::update() {
    trlt::UpdateNodePacket packet;
    system("nano update.conf");
    std::map<std::string, std::string> settings;
    settings["Node_Name"] = "";
    settings["Mode"] = "";
    settings["Node_Value"] = "";
    settings["Attribute_Name"] = "";
    settings["Attribute_Value"] = "";
    
    if(tool::readSettingsFile("update.conf", settings)){

    }
    return true;
  }

  bool Client::drop() {
    trlt::DeleteNodePacket packet;
    system("nano drop.conf");
    std::map<std::string, std::string> settings;
    settings["Mode"] = "";
    settings["Node_Name"] = "";
    settings["Attribute/Relation"] = "";
    if(tool::readSettingsFile("drop.conf", settings)){

    }
    return true;
  }
}

#endif //CLIENT_PROTOCOL_HANDLER_HPP_