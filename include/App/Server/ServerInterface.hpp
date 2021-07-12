#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <tuple>
#include <utility>
#include <vector>

#include "../../Network/RDTListener.hpp"
#include "../../Network/RDTSocket.hpp"
#include "../Tools/InterfacePerformance.hpp"
#include "../Tools/Colors.hpp"

#define REPOSITORY_LIMIT   1000

namespace app{

  class ServerMaster {
  private:
    rdt::RDTListener clientListener, repositoryListener;
    std::vector<std::pair<uint16_t, std::string>> repositoryConnectionPool;

    void connEnvironmentClient(std::shared_ptr<rdt::RDTSocket> socket);
    void connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket> socket);
  
    void runRepositoryListener();

    std::mutex alternateConsolePrintMutex, repositoryPoolMutex;
  public:
    ServerMaster(const uint16_t& listenerPortClient, const uint16_t& listenerPortRepository);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_