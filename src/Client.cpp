// Client.cpp
#include <iostream>
#include "Network/Socket.hpp"
#include "App/App.hpp"

int main(int argc, char *argv[])
{
  app::Client clientInstance("34.94.147.12", "8000");
  //RDT::UdpSocket clientSocket("34.94.147.12", "8000");

  if(argc > 1)
  {
    if(!clientInstance.setCommand(argv[1]))
    {
      tool::ConsolePrint("[Error]: Can not process this command", RED);
      return EXIT_FAILURE;
    }
  }
  //clientSocket.send();

  return EXIT_SUCCESS;
}
