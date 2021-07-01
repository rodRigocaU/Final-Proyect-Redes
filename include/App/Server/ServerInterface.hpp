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
    std::map<int32_t, std::shared_ptr<rdt::RDTSocket>> repositoryConnectionPool;
    std::map<int32_t, std::shared_ptr<rdt::RDTSocket>> clientConnectionPool;

    void connEnvironmentClient(std::shared_ptr<rdt::RDTSocket>& socket);
    void connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket>& socket);
  public:
    ServerMaster(const uint16_t& listenerPort);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_