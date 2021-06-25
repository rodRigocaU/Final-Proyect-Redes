#include "Network/RDTSocket.hpp"
#include "Network/Algorithm/sha256.hpp"
#include "App/Tools/Fixer.hpp"
#include <iomanip>

#include <math.h>

// vea el siguiente link: https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
float rdt::RDTSocket::RTTEstimator::EWMA(float constant, float firstTerm, float secondTerm)
{
  return (1 - constant) * firstTerm + constant * secondTerm;
}

void rdt::RDTSocket::RTTEstimator::estRTT()
{
  EstimatedRTT = static_cast<int>(EWMA(0.125, static_cast<float>(EstimatedRTT), static_cast<float>(SampleRTT)));
}

void rdt::RDTSocket::RTTEstimator::varRTT()
{
  DevRTT = static_cast<int>(EWMA(0.25, static_cast<float>(DevRTT), static_cast<float>(SampleRTT - EstimatedRTT)));
}

int rdt::RDTSocket::RTTEstimator::operator()(int _SampleRTT)
{
  if (_SampleRTT > 0) SampleRTT = _SampleRTT;
  estRTT();                         // primero obtenemos el estimated RTT
  varRTT();                         // luego su variación
  return (EstimatedRTT + 4 * DevRTT < 0)? DEFAULT_RTT : EstimatedRTT + 4 * DevRTT; // esto es lo que debemos esperar aproximadamente
}

rdt::RDTSocket::RTTEstimator::~RTTEstimator() {}

rdt::RDTSocket::RDTSocket(){
  alterBit = ALTERBIT_LOWERBOUND;
  connectionStatus = net::Status::Disconnected;
}

rdt::RDTSocket::~RDTSocket(){
  disconnect();
}

const uint16_t& rdt::RDTSocket::getLocalPort() const{
  return connectionInfo.localPort;
}

const std::string& rdt::RDTSocket::getRemoteIpAddress() const{
  return connectionInfo.remoteIp;
}

const uint16_t& rdt::RDTSocket::getRemotePort() const{
  return connectionInfo.remotePort;
}

int32_t rdt::RDTSocket::getSocketFileDescriptor() const{
  if(mainSocket != nullptr)
    return mainSocket->socketId;
  return -1;
}

uint8_t rdt::RDTSocket::switchBitAlternate(){
  uint8_t tempMod = ALTERBIT_UPPERBOUND + 1 - ALTERBIT_LOWERBOUND;
  alterBit = (alterBit - ALTERBIT_LOWERBOUND + 1) % tempMod;
  return alterBit += ALTERBIT_LOWERBOUND;
}

std::string rdt::RDTSocket::encode(const std::string& message){
  std::string sha256Hash = crypto::sha256(message);
  std::string messageSize = tool::fixToBytes(std::to_string(message.length()), MSG_BYTE_SIZE);
  std::string encodedMsg = tool::fixToBytes(std::to_string(alterBit), ALTERBIT_BYTE_SIZE) + sha256Hash + messageSize + message;
  tool::paddingPacket(encodedMsg, '0', net::MAX_DGRAM_SIZE);
  return encodedMsg;
}

bool rdt::RDTSocket::decode(std::string& message){
  std::string header = message.substr(0, RDT_HEADER_BYTE_SIZE);
  message = message.substr(RDT_HEADER_BYTE_SIZE, std::stoi(header.substr(ALTERBIT_BYTE_SIZE + HASH_BYTE_SIZE, MSG_BYTE_SIZE)));
  return (std::stoi(header.substr(0, ALTERBIT_BYTE_SIZE)) == alterBit) &&
  (header.substr(ALTERBIT_BYTE_SIZE, HASH_BYTE_SIZE) == crypto::sha256(message));
}

void rdt::RDTSocket::setTimerConfigurations(){
  if(mainSocket != nullptr){
    timer[0].fd = mainSocket->socketId;
    timer[0].events = POLLIN;
  }
}

net::Status rdt::RDTSocket::connect(const std::string& remoteIp, const uint16_t& remotePort){
  mainSocket = std::make_unique<net::UdpSocket>();
  setTimerConfigurations();
  connectionInfo.remoteIp = remoteIp;
  connectionInfo.remotePort = remotePort;
  connectionStatus = net::Status::Disconnected;
  std::string intentResponse;
  //SENDING SOCKET CLIENT CREDENTIALS
  if(send("PLOX") != net::Status::Done){
    disconnect();
    return net::Status::Error;
  }
  //RECEIVING SOCKET SERVER CREDENTIALS
  if(receive(intentResponse) != net::Status::Done){
    disconnect();
    return net::Status::Error;
  }
  if(intentResponse != "PASS"){
    disconnect();
    return net::Status::Error;
  }
  connectionStatus = net::Status::Connected;
  return net::Status::Done;
}

void rdt::RDTSocket::disconnect(){
  if(mainSocket != nullptr){
    mainSocket->unbind();
    mainSocket.reset();
    connectionStatus = net::Status::Disconnected;
  }
}

net::Status rdt::RDTSocket::bindPort(const uint16_t& localPort){
  if(mainSocket == nullptr)
    mainSocket = std::make_unique<net::UdpSocket>();
  if(mainSocket->bind(localPort) != net::Status::Done)
    return net::Status::Error;
  connectionInfo.localIp = mainSocket->localIp;
  connectionInfo.localPort = mainSocket->localPort;
  setTimerConfigurations();
  return net::Status::Done;
}

bool rdt::RDTSocket::existMessagesWaiting(){
  return timer[0].revents & POLLIN;
}

net::Status rdt::RDTSocket::secureSend(std::string& packet, const uint8_t& expectedAlterBit) {
  if(mainSocket != nullptr){
    std::string remoteIp;
    uint16_t remotePort;
    bool successSending = false;

    int32_t EsRTT = estimateRTT(); // TO DO: Función para calcular el RTT
    std::chrono::high_resolution_clock::time_point t_init;
    std::chrono::high_resolution_clock::time_point t_end;
    
    do{
      successSending = false;
      if(mainSocket->send(packet, connectionInfo.remoteIp, connectionInfo.remotePort) != net::Status::Done)
        return net::Status::Error;

      t_init = std::chrono::high_resolution_clock::now();
      int32_t responseTimeCode = poll(timer, 1, EsRTT);
      t_end = std::chrono::high_resolution_clock::now();
      EsRTT = estimateRTT(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_init).count());
      
      if(responseTimeCode == -1)
        return net::Status::Error;
      else if(responseTimeCode == 0)
        continue;
      else {
        if(existMessagesWaiting()) {
          std::string ACK;
          mainSocket->receive(ACK, remoteIp, remotePort);
          ACK = ACK.substr(0, ALTERBIT_BYTE_SIZE);
          if(expectedAlterBit != std::stoi(ACK))
            continue;
          successSending = true;
        }
      }
    } while(!successSending);
    return net::Status::Done;
  }
  return net::Status::Disconnected;
}

net::Status rdt::RDTSocket::secureRecv(std::string& packet, const uint8_t& expectedAlterBit){
  if(mainSocket != nullptr){
    std::string remoteIp;
    uint16_t remotePort;
    bool successReceiving = false;
    std::size_t bytes_sent;
    do {
      mainSocket->receive(packet, remoteIp, remotePort);
      if(connectionStatus == net::Status::Disconnected){
        connectionInfo.remoteIp = remoteIp;
        connectionInfo.remotePort = remotePort;
      }
      successReceiving = decode(packet);

      if(successReceiving) {
        std::string ACK = tool::fixToBytes(std::to_string(alterBit), ALTERBIT_BYTE_SIZE);
        tool::paddingPacket(ACK, '0', net::MAX_DGRAM_SIZE);
        if(mainSocket->send(ACK, connectionInfo.remoteIp, connectionInfo.remotePort) != net::Status::Done)
          return net::Status::Error;
      }
    } while (!successReceiving);
    return net::Status::Done;
  }
  return net::Status::Disconnected;
}

net::Status rdt::RDTSocket::send(const std::string& message){
  net::Status connectionStatus;
  const uint64_t BODY_MSG_BYTE_SIZE = net::MAX_DGRAM_SIZE - RDT_HEADER_BYTE_SIZE;
  uint64_t packetCount = std::ceil(double(message.length()) / double(BODY_MSG_BYTE_SIZE));

  std::string packetCountEncoded = encode(std::to_string(packetCount));
  connectionStatus = secureSend(packetCountEncoded, alterBit);
  if(connectionStatus != net::Status::Done)
    return connectionStatus;
  switchBitAlternate();

  for(uint64_t i = 0, j = 0; i < packetCount; ++i, j += BODY_MSG_BYTE_SIZE) {
    std::string packetChunk = encode(message.substr(j, BODY_MSG_BYTE_SIZE));
    connectionStatus = secureSend(packetChunk, alterBit);
    if(connectionStatus != net::Status::Done)
      return connectionStatus;
    switchBitAlternate();
  }
  return net::Status::Done; 
}

net::Status rdt::RDTSocket::receive(std::string& message){
  message.clear();
  net::Status connectionStatus;
  std::string nSubPackets;
  connectionStatus = secureRecv( nSubPackets, alterBit);
  if(connectionStatus != net::Status::Done)
    return connectionStatus;
  switchBitAlternate();
  uint64_t packetCount = std::stoi( nSubPackets);
  for(uint64_t i = 0; i < packetCount; ++i){
    std::string packetChunk;
    connectionStatus = secureRecv(packetChunk, alterBit);
    if(connectionStatus != net::Status::Done)
      return connectionStatus;
    message += packetChunk;
    switchBitAlternate();
  }
  return net::Status::Done;
}

std::ostream& operator<<(std::ostream& out, const rdt::RDTSocket& socket){
  out << "+--------------------+\n";
  out << "|RDT::Reliable Socket|\n";
  out << "+--------------------+\n";
  out << "|FD: " << std::setw(16) << socket.getSocketFileDescriptor() << "|\n";
  out << "|Ip: " << std::setw(16) << socket.getRemoteIpAddress() << "|\n";
  out << "|Port: " << std::setw(14) << socket.getRemotePort() << "|\n";
  out << "+--------------------+\n";
  return out;
}