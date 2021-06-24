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
  if(socket.connect("127.0.0.1", 5001) != net::Status::Done){
    return EXIT_FAILURE;
  }
  socket.send("Hola como te va");
  std::string msg;
  socket.receive(msg);
  std::cout << msg << std::endl;

  return EXIT_SUCCESS;
}
