#include "Network/RDTSocket.hpp"
#include "Network/RDTListener.hpp"
#include "App/Server/ServerInterface.hpp"

int main(){
  rdt::RDTListener listener;
  if(listener.listen(5001) != net::Status::Done){
    return EXIT_FAILURE;
  }
  std::cout << listener << std::endl;
  rdt::RDTSocket socket;
  if(listener.accept(socket) != net::Status::Done){
    return EXIT_FAILURE;
  }
  std::cout << socket << std::endl;
  std::string msg;
  std::cout << "+-+-+-++-+-+RECV 1\n";
  socket.receive(msg);
  std::cout << "==========MENSAJE: " << msg << std::endl;
  std::cout << "+-+-+-++-+-+SEND 1\n";
  socket.send("AEA MANO");
  std::cout << "+-+-+-++-+-+SEND 2\n";
  socket.receive(msg);
  socket.send("RAAAA moment");
  std::cout << "+-+-+-+PROCEED TO DISCONNECT\n";
  socket.passiveDisconnect();
  /*net::UdpSocket slaveServerSocket("35.188.208.43", "8000");

  std::string received_message, IP_from;
  uint16_t Port_from;
  mainServerListener.receive(received_message, IP_from, Port_from);
  slaveServerSocket.send(received_message);*/
  /*
  
  while(true){
    std::unique_ptr<RDTSocket> newSocket = std::make_unique<RDTSocket>();
    
  }

  */
  return 0;
}
