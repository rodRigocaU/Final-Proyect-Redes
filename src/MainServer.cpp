#include "Network/RDTSocket.hpp"
#include "Network/RDTListener.hpp"
#include "App/Server/ServerInterface.hpp"
#include <thread>

void recv_messages(rdt::RDTSocket& curr_socket)
{
  std::string msg;
  while(true){
    curr_socket.receive(msg);
    std::cout << "message: " << msg << "\n";
    std::cout << "Para: " << curr_socket << std::endl;
  }
}


int main(){
  rdt::RDTListener listener;
  if(listener.listen(5001) != net::Status::Done){
    return EXIT_FAILURE;
  }

  std::vector<std::unique_ptr<rdt::RDTSocket>> comm_sockets;
  while (true)
  {
    rdt::RDTSocket tmp;
    if (listener.accept(tmp) != net::Status::Done)
    {
      return EXIT_FAILURE;
    }
    std::cout << "nueva conexión: " << tmp << std::endl;
    comm_sockets.push_back(std::make_unique<rdt::RDTSocket>(tmp));
    std::thread(recv_messages, std::ref(**comm_sockets.end())).detach();   
  }

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
