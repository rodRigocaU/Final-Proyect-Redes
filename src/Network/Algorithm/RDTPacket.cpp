#include "App/Tools/Fixer.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "Network/RDTSocket.hpp"
/*
  +---------------------------------------+
  |                 SHA256                |[64 bytes]
  +---------------------------------------+
  | =PacketType  |  =ACK   |=MessageSize  |[1 byte][1 byte][3 bytes]
  +---------------------------------------+
  |       =MessageBody        | =Padding  |[(0,931) bytes][1000 - (MessageSize + header)]
  +---------------------------------------+
*/

std::string rdt::RDTSocket::RDTPacket::encode(const std::string& message, const uint16_t& currentACK, const Type& interpretation){
  ack = currentACK;
  packetType = interpretation;
  msgBody = message;
  std::string strPacketType = tool::fixToBytes(std::to_string(interpretation), PACKET_TYPE_BYTE_SIZE);
  std::string strACK = tool::fixToBytes(std::to_string(ack), ALTERBIT_BYTE_SIZE);
  std::string strMsgSize = tool::fixToBytes(std::to_string(message.length()), MSG_BYTE_SIZE);
  std::string toEncode = strPacketType + strACK + strMsgSize + message;
  tool::paddingPacket(toEncode, '0', net::MAX_DGRAM_SIZE - HASH_BYTE_SIZE);
  std::string sha256Hash = crypto::sha256(toEncode);
  return sha256Hash + toEncode;
}

void rdt::RDTSocket::RDTPacket::decode(const std::string& encoded){
  std::string header = encoded.substr(0, RDT_HEADER_BYTE_SIZE);
  corrupted = header.substr(0, HASH_BYTE_SIZE) != crypto::sha256(encoded.substr(HASH_BYTE_SIZE));
  if(!corrupted){
    packetType = static_cast<Type>(std::stoi(header.substr(HASH_BYTE_SIZE, PACKET_TYPE_BYTE_SIZE)));
    ack = std::stoi(header.substr(HASH_BYTE_SIZE + PACKET_TYPE_BYTE_SIZE, ALTERBIT_BYTE_SIZE));
    std::size_t messageSize = std::stoul(header.substr(HASH_BYTE_SIZE + PACKET_TYPE_BYTE_SIZE + ALTERBIT_BYTE_SIZE, MSG_BYTE_SIZE));
    msgBody = encoded.substr(RDT_HEADER_BYTE_SIZE, messageSize);
  }
  else{
    packetType = Type::Information;
    ack = 0;
    msgBody = "";
  }
}

bool rdt::RDTSocket::RDTPacket::isCorrupted(){
  return corrupted;
}

bool rdt::RDTSocket::RDTPacket::isSynchronized(const uint8_t& expectedACK){
  return ack == expectedACK;
}

const rdt::RDTSocket::RDTPacket::Type& rdt::RDTSocket::RDTPacket::getPacketType(){
  return packetType;
}

const std::string& rdt::RDTSocket::RDTPacket::getMessageBody(){
  return msgBody;
}

const uint16_t& rdt::RDTSocket::RDTPacket::getACK(){
  return ack;
}