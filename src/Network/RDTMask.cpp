#include "Network/RDTMask.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "App/Tools/Fixer.hpp"

using namespace rdt;

uint8_t RDTSocket::switchBitAlternate(){
  uint8_t tempMod = ALTERBIT_UPPERBOUND + 1 - ALTERBIT_LOWERBOUND;
  alterBit = (alterBit - ALTERBIT_LOWERBOUND + 1) % tempMod;
  return alterBit += ALTERBIT_LOWERBOUND;
}

bool RDTSocket::isCorrupted(const std::string& message, const std::string& hash){
  return crypto::sha256(message) == hash;
}

void RDTSocket::encode(std::string& message){
  std::string sha256Hash = crypto::sha256(message);
  std::string messageSize = tool::fixToBytes(std::to_string(message.length()), 4);
  message = std::to_string(alterBit) + sha256Hash + messageSize + message;
  switchBitAlternate();
}

bool RDTSocket::decode(std::string& message){
  std::string header = message.substr(0, RDT_HEADER_BYTE_SIZE);
  message = message.substr(RDT_HEADER_BYTE_SIZE);
  return (std::stoi(header.substr(0, ALTERBIT_BYTE_SIZE)) == alterBit) &&
  (header.substr(ALTERBIT_BYTE_SIZE, HASH_BYTE_SIZE) == crypto::sha256(message));
}

void RDTSocket::setReceptorSettings(const std::string& IpAddress, const uint16_t& port){
  mainSocket = std::make_unique<net::UdpSocket>(IpAddress, port);
}

RDTSocket::Status RDTSocket::send(const std::string& message){
  return Done;
}
RDTSocket::Status RDTSocket::receive(std::string& message, std::string& remoteIp, uint16_t& remotePort){
  return Done;
}