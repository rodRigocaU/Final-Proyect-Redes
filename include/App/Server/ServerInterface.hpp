#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#include "../../Network/RDTSocket.hpp"

namespace app{

  class MainServer {
  private:
    rdt::RDTSocket listener;
    std::vector<rdt::RDTSocket> repositoryPool;
  public:
    MainServer(const uint16_t& localPort);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_