#include "App/Tools/Fixer.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "Network/RDTSocket.hpp"
/*
  +---------------------------------------+
  |                 SHA256                |[64 bytes]
  +---------------------------------------+
  |  PacketType  |   ACK   | MessageSize  |[1 byte][1 byte][3 bytes]
  +---------------------------------------+
  |        MessageBody        |  Padding  |[(0,931) bytes][1000 - (MessageSize + header)]
  +---------------------------------------+
*/

const std::string& rdt::RDTSocket::RDTPacket::encode(const std::string& message, const uint8_t& currentACK, const Type& interpretation){
  ack = currentACK;
  packetType = interpretation;
  msgBody = message;
  std::string strPacketType = tool::fixToBytes(std::to_string(interpretation), PACKET_TYPE_BYTE_SIZE);
  std::string strACK = tool::fixToBytes(std::to_string(ack), ALTERBIT_BYTE_SIZE);
  std::string strMsgSize = tool::fixToBytes(std::to_string(message.length()), MSG_BYTE_SIZE);;
  std::string sha256Hash = crypto::sha256(strPacketType + strACK + strMsgSize + message);
  std::string encoded = sha256Hash + strPacketType + strACK + strMsgSize + message;
  tool::paddingPacket(encoded, '0', net::MAX_DGRAM_SIZE);
  return encoded;
}

void rdt::RDTSocket::RDTPacket::decode(const std::string& encoded){
  std::string header = encoded.substr(0, RDT_HEADER_BYTE_SIZE);
  packetType = static_cast<Type>(std::stoi(header.substr(HASH_BYTE_SIZE, PACKET_TYPE_BYTE_SIZE)));
  ack = std::stoi(header.substr(HASH_BYTE_SIZE + PACKET_TYPE_BYTE_SIZE, ALTERBIT_BYTE_SIZE));
  std::size_t messageSize = std::stoul(header.substr(HASH_BYTE_SIZE + PACKET_TYPE_BYTE_SIZE + ALTERBIT_BYTE_SIZE, MSG_BYTE_SIZE));
  msgBody = encoded.substr(RDT_HEADER_BYTE_SIZE, messageSize);
  corrupted = header.substr(0, HASH_BYTE_SIZE) != crypto::sha256(header.substr(HASH_BYTE_SIZE, RDT_HEADER_BYTE_SIZE - HASH_BYTE_SIZE) + msgBody);
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

const uint8_t& rdt::RDTSocket::RDTPacket::getACK(){
  return ack;
}