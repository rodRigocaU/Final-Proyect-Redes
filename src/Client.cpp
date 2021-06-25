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
  std::string text;
  for(int i = 0; i < 2000; ++i){
    text += std::to_string(i);
  }
  socket.send(text);

  std::string msg;
  socket.receive(msg);
  std::cout << "Message: " << msg << std::endl;
  std::cout << "IP from: " << socket.getRemoteIpAddress() << std::endl;
  std::cout << "Port from: " << socket.getRemotePort() << std::endl;
  std::cout << socket << std::endl;
  return EXIT_SUCCESS;
}
