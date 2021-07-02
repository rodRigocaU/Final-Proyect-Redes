#include "Network/RDTMask.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "App/Tools/Fixer.hpp"

#include <math.h>

namespace rdt{
  // vea el siguiente link: https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
  float RDTSocket::RTTEstimator::EWMA(float constant, float firstTerm, float secondTerm) {
    return (1 - constant) * firstTerm + constant * secondTerm;
  }

  void RDTSocket::RTTEstimator::estRTT() {
    EstimatedRTT = static_cast<int>(EWMA(0.125, static_cast<float>(EstimatedRTT), static_cast<float>(SampleRTT)));
  }

  void RDTSocket::RTTEstimator::varRTT() {
    DevRTT = static_cast<int>(EWMA(0.25, static_cast<float>(DevRTT), static_cast<float>(SampleRTT - EstimatedRTT)));
  }
  
  int RDTSocket::RTTEstimator::operator()(int _SampleRTT) {
    if(!SampleRTT) SampleRTT = _SampleRTT;
    estRTT(); // primero obtenemos el estimated RTT
    varRTT(); // luego su variación
    return EstimatedRTT + 4 * DevRTT; // esto es lo que debemos esperar aproximadamente
  }

  RDTSocket::RTTEstimator::~RTTEstimator() {}


  uint8_t RDTSocket::switchBitAlternate(){
    uint8_t tempMod = ALTERBIT_UPPERBOUND + 1 - ALTERBIT_LOWERBOUND;
    alterBit = (alterBit - ALTERBIT_LOWERBOUND + 1) % tempMod;
    return alterBit += ALTERBIT_LOWERBOUND;
  }

  bool RDTSocket::isCorrupted(const std::string& message, const std::string& hash){
    return message != hash;
  }

  std::string RDTSocket::encode(const std::string& message){
    std::string sha256Hash = crypto::sha256(message);
    std::string messageSize = tool::fixToBytes(std::to_string(message.length()), 4);
    std::string encodedMsg = tool::fixToBytes(std::to_string(alterBit), ALTERBIT_BYTE_SIZE) + sha256Hash + messageSize + message;
    tool::followUpPacket(encodedMsg, '0', net::MAX_DGRAM_SIZE);
    return encodedMsg;
  }

  bool RDTSocket::decode(std::string& message){
    std::string header = message.substr(0, RDT_HEADER_BYTE_SIZE);
    message = message.substr(RDT_HEADER_BYTE_SIZE, std::stoi(header.substr(ALTERBIT_BYTE_SIZE + HASH_BYTE_SIZE, MSG_BYTE_SIZE)));
    return (std::stoi(header.substr(0, ALTERBIT_BYTE_SIZE)) == alterBit) &&
    (!isCorrupted(header.substr(ALTERBIT_BYTE_SIZE, HASH_BYTE_SIZE), crypto::sha256(message)));
  }




  RDTSocket::RDTSocket() {}

  RDTSocket::RDTSocket(const std::string& IpAddress, const uint16_t& port) {
    mainSocket.configureSocket(IpAddress, std::to_string(port));
    timer[0].fd = mainSocket.getSocketFileDescriptor();
    timer[0].events = POLLIN;
  }

  void RDTSocket::setReceptorSettings(const std::string& IpAddress, const uint16_t& port){
    mainSocket.configureSocket(IpAddress, std::to_string(port));
    timer[0].fd = mainSocket.getSocketFileDescriptor();
    timer[0].events = POLLIN;
  }

  uint8_t RDTSocket::getBitAlternate(u_char* buffer){
    std::string ack_alterBit;
    for(std::size_t i = 0; i < ALTERBIT_BYTE_SIZE; ++i)
      ack_alterBit += buffer[i];
    return std::stoi(ack_alterBit);
  }

  RDTSocket::Status RDTSocket::secureSend(std::string& packet, const uint8_t& expectedAlterBit) {
    if(mainSocket.active()){
      int32_t bytes_sent = 0; // la cantidad de bytes enviados
      bool reached_correct_to_dest = false;

      int32_t timeOut = estimateRTT(); // TO DO: Función para calcular el RTT
      u_char recv_buffer[net::MAX_DGRAM_SIZE];
      auto t_init = std::chrono::high_resolution_clock::now();
      auto t_end = std::chrono::high_resolution_clock::now();
      do{
        reached_correct_to_dest = false;
        char temp[net::MAX_DGRAM_SIZE + 1];
        strcpy(temp, packet.c_str());
        
        if(mainSocket.sendAll(reinterpret_cast<u_char*>(temp), bytes_sent, false) == -1)
          return Error;

        t_init = std::chrono::high_resolution_clock::now();
        
        int32_t ret = poll(timer, 1, timeOut);
        
        t_end = std::chrono::high_resolution_clock::now();
        // timeout estimado para el siguiente envío
        timeOut = estimateRTT(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_init).count());
        
        if(ret == -1)
          return Error;
        else if(ret == 0) {// timeout
          continue;
        }
        else {
          if(timer[0].revents & POLLIN) {
            mainSocket.simpleRecv(recv_buffer);
            if(expectedAlterBit != getBitAlternate(recv_buffer))
              continue;

            reached_correct_to_dest = true;
          }
        }
      }while(!reached_correct_to_dest);
      return Done;
    }
    return Disconnected;
  }

  RDTSocket::Status RDTSocket::secureRecv(std::string& packet, const uint8_t& expectedAlterBit){
    if(mainSocket.active()){
      u_char packetBuffer[net::MAX_DGRAM_SIZE + 1];
      bool correct_packet;
      int32_t bytes_sent;
      do {
        if(mainSocket.simpleRecv(packetBuffer) == -1)
        {
          perror("simpleRecv");
        }

        packetBuffer[net::MAX_DGRAM_SIZE] = '\0';
        packet = std::string(reinterpret_cast<char*>(packetBuffer));
        correct_packet = decode(packet);

        if(correct_packet) {
          char temp[net::MAX_DGRAM_SIZE + 1];
          std::string ACK = tool::fixToBytes(std::to_string(alterBit), ALTERBIT_BYTE_SIZE);
          tool::followUpPacket(ACK, '0', net::MAX_DGRAM_SIZE);
          strcpy(temp, ACK.c_str());
          if(mainSocket.sendAll(reinterpret_cast<u_char*>(temp), bytes_sent, true) == -1)
            return Error;
        }
      } while (!correct_packet);
      return Done;
    }
    return Disconnected;
  }

  RDTSocket::Status RDTSocket::send(const std::string& message){
    Status connectionStatus;
    const uint64_t BODY_MSG_BYTE_SIZE = net::MAX_DGRAM_SIZE - RDT_HEADER_BYTE_SIZE;
    uint64_t packetCount = std::ceil(double(message.length()) / double(BODY_MSG_BYTE_SIZE));

    std::string quantity_of_packets = encode(std::to_string(packetCount));
    connectionStatus = secureSend(quantity_of_packets, alterBit);
    if(connectionStatus != Done)
      return connectionStatus;
    switchBitAlternate();

    for(uint64_t i = 0, j = 0; i < packetCount; ++i, j += BODY_MSG_BYTE_SIZE) {
      std::string packet_j = encode(message.substr(j, BODY_MSG_BYTE_SIZE));
      connectionStatus = secureSend(packet_j, alterBit);
      if(connectionStatus != Done)
        return connectionStatus;
      switchBitAlternate();
    }
    return Done; 
  }
  RDTSocket::Status RDTSocket::receive(std::string& message, std::string& remoteIp, uint16_t& remotePort){
    message.clear();
    remoteIp.clear();
    remotePort = 0x0000;
    Status connectionStatus;
    std::string quantity_of_packets;
    connectionStatus = secureRecv(quantity_of_packets, alterBit);
    if(connectionStatus != Done)
    {
      return connectionStatus;
    }

    switchBitAlternate();
    uint64_t packetCount = std::stoi(quantity_of_packets);
    for(uint64_t i = 0; i < packetCount; ++i){
      std::string packet_i;
      connectionStatus = secureRecv(packet_i, alterBit);
      if(connectionStatus != Done)
        return connectionStatus;
      message += packet_i;
      switchBitAlternate();
    }

    remoteIp = mainSocket.getSenderIP();  
    remotePort = mainSocket.getSenderPort();
    return Done;
  }

}