#include "App/Client/ClientInterface.hpp"
#include "App/Tools/Colors.hpp"
#include "App/Tools/Fixer.hpp"
#include "App/Tools/InterfacePerformance.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"

namespace app{

  Client::Client(const std::string& serverIp, const std::string& serverPort) {
    commands["spawn"]  = std::bind(&Client::create, this);
    commands["ask"]    = std::bind(&Client::read, this);
    commands["update"] = std::bind(&Client::update, this);
    commands["drop"]   = std::bind(&Client::drop, this);

    if(clientSocket.connect(serverIp, std::stoi(serverPort)) != net::Status::Done){
      tool::ConsolePrint("=> Unexpected error, can not connect with server master.\n", RED);
      exit(EXIT_FAILURE);
    }
  }

  bool Client::setCommand(const std::string& command) {
    std::unordered_map<std::string, std::function<bool(void)>>::iterator function = commands.find(command);
    if(function != commands.end())
      return function->second();
    return false;
  }

  bool Client::create() {
    msg::CreateNodePacket packet;
    system("nano spawn.conf");
    std::map<std::string, std::string> settings;
    settings[CENAPSE_CODE_NODE_NAME] = "";
    settings[CENAPSE_CODE_ATRIBUTES] = "";
    settings[CENAPSE_CODE_RELATIONS] = "";
    std::string message;
    if(tool::readSettingsFile("spawn.conf", settings)){
      packet << settings;
      packet >> message;
    }
<<<<<<< HEAD
    clientSocket.send(message);
=======
    remoteSocket.send(message);
>>>>>>> 8a9ce7b33de0d02a90953e083d181088810e3d53
    return true;
  }

  bool Client::read() {    
    msg::ReadNodePacket packet;
    system("nano ask.conf");
    std::map<std::string, std::string> settings;
    settings[CENAPSE_CODE_NODE_NAME] = "";
    settings[CENAPSE_CODE_DEPTH] = "";
    settings[CENAPSE_CODE_LEAF] = "";
    settings[CENAPSE_CODE_ATRIBUTES_REQUIRED];
    settings[CENAPSE_CODE_QUERY_FEATURES] = "";
    std::string message;
    if(tool::readSettingsFile("ask.conf", settings)){
      packet << settings;
      packet >> message;
    }
<<<<<<< HEAD
    clientSocket.send(message);
=======
    remoteSocket.send(message);
>>>>>>> 8a9ce7b33de0d02a90953e083d181088810e3d53
    return true;
  }

  bool Client::update() {
    msg::UpdateNodePacket packet;
    system("nano update.conf");
    std::map<std::string, std::string> settings;
    settings[CENAPSE_CODE_NODE_NAME] = "";
    settings[CENAPSE_CODE_NA_MODE] = "";
    settings[CENAPSE_CODE_NODE_VALUE] = "";
    settings[CENAPSE_CODE_ATRIBUTE_I_VALUE] = "";
    std::string message;
    if(tool::readSettingsFile("update.conf", settings)){
      packet << settings;
      packet >> message;
    }
<<<<<<< HEAD
    clientSocket.send(message);
=======
    remoteSocket.send(message);
>>>>>>> 8a9ce7b33de0d02a90953e083d181088810e3d53
    return true;
  }

  bool Client::drop() {
    msg::DeleteNodePacket packet;
    system("nano drop.conf");
    std::map<std::string, std::string> settings;
    settings[CENAPSE_CODE_NAR_MODE] = "";
    settings[CENAPSE_CODE_NODE_NAME] = "";
    settings[CENAPSE_CODE_ATRIBUTE_I_RELATION] = "";
    std::string message;
    if(tool::readSettingsFile("drop.conf", settings)){
      packet << settings;
      packet >> message;
    }
    clientSocket.send(message);
    return true;
  }

}