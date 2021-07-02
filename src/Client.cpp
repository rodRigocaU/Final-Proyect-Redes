#include <iostream>
#include "App/App.hpp"
#include "Network/RDTSocket.hpp"

int main(int argc, char *argv[]) {
  /*
  app::Client clientInstance("34.94.147.12", "8000");

  if(argc > 1) {
    if(!clientInstance.setCommand(argv[1])) {
      tool::ConsolePrint("[Error]: Can not process this command", RED);
      return EXIT_FAILURE;
    }
  }*/

  rdt::RDTSocket socket;
  if(socket.connect("34.94.147.12", 5001) != net::Status::Done){
    return EXIT_FAILURE;
  }
  
  std::cout << socket;

  while(true){
    std::string msg;
    std::cout << "Ingresa un mensaje al server: ";
    std::getline(std::cin, msg);
    socket.send(msg);
  }

  socket.disconnectInitializer();
  return EXIT_SUCCESS;
}
