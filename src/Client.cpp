#include <iostream>
#include "App/App.hpp"
#include "Network/RDTMask.hpp"
#include "Network/UdpSocket.hpp"


int main(int argc, char *argv[]) {

  rdt::RDTSocket clientSocket("127.0.0.1", 8000);
  //clientSocket.setReceptorSettings("127.0.0.1", 8000);
  

  std::string text_msg;

	for(int i = 0; i < 1000; ++i){
		text_msg += std::to_string(i);
	}
	std::cout << "tamaño del mensaje: " << text_msg.size() << std::endl;

  clientSocket.send(text_msg);


  //net::UdpSocket clientSimpleSocket("127.0.0.1", "8000");
  // someSocket clientSimpleSocket("127.0.0.1", "8000");
  // u_char message[] = "hola como estas";
  // int bytes_sent;

  // clientSimpleSocket.socket.sendAll(message, bytes_sent, false);


  // app::Client clientInstance("34.94.147.12", "8000");

  // if(argc > 1) {
  //   if(!clientInstance.setCommand(argv[1])) {
  //     tool::ConsolePrint("[Error]: Can not process this command", RED);
  //     return EXIT_FAILURE;
  //   }
  // }

  return EXIT_SUCCESS;
}
