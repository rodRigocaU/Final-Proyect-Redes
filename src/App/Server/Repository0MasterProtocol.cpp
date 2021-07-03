#include "App/Server/Repository0MasterProtocol.hpp"
#include <sstream>

std::pair<uint64_t, std::list<uint64_t>> tool::parseRepoActiveCommand(const std::string& commandBody){
  std::stringstream buffer;
  buffer << commandBody;
  std::string item;
  std::getline(buffer, item, COMMAND_SEPARATOR);
  uint64_t netNodeId = std::stoull(item);
  std::list<uint64_t> nodeList;
  while(std::getline(buffer, item, COMMAND_SPLIT)){
    nodeList.push_back(std::stoull(item));
  }
  return {netNodeId, nodeList};
}