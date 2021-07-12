#include "App/Server/Repository0MasterProtocol.hpp"
#include <sstream>

std::list<uint64_t> tool::parseRepoActiveCommand(const std::string& commandBody){
  std::stringstream buffer;
  buffer << commandBody;
  std::string item;
  std::list<uint64_t> nodeList;
  while(std::getline(buffer, item, COMMAND_SPLIT)){
    nodeList.push_back(std::stoull(item));
  }
  return nodeList;
}