#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"

namespace app{

  class ServerMaster {
  private:
    rdt::RDTListener listener;
    std::vector<rdt::RDTSocket> repositoryPool;
    std::map<int32_t, std::shared_ptr<rdt::RDTSocket>> clientConnectionPool;
  public:
    ServerMaster(const uint16_t& listenerPort);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_