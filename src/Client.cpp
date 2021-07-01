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
  std::cout << socket << std::endl;
  std::string msg;
  socket.receive(msg);
  std::cout << "==========MENSAJE: " << msg << std::endl;
  socket.send("SOY GUAPO LO SE");
  std::cout << "==========MENSAJE: " << msg << std::endl;
  std::cout << "+-+-+-++-+-+SEND 1\n";
  socket.send("hectiris@gmail.com");
  std::cout << "+-+-+-++-+-+RECV 1\n";
  socket.receive(msg);
  std::cout << "==========MENSAJE: " << msg << std::endl;
  std::cout << "+-+-+-++-+-+RECV 2\n";
  socket.send("Hola como te va");
  socket.receive(msg);
  std::cout << "==========MENSAJE: " << msg << std::endl;
  socket.send("Bueno ya me voy");
  std::cout << "++++PROCEED TO DISCONNECT\n";
  socket.disconnectInitializer();
  return EXIT_SUCCESS;
}
