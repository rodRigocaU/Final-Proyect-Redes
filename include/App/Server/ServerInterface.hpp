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
#include "Repository0MasterProtocol.hpp"

#define REPOSITORY_LIMIT   1000

namespace app{

  class ServerMaster {
  private:
    rdt::RDTListener clientListener, repositoryListener;
    std::map<tool::IdNetNode, std::pair<std::pair<uint16_t, uint16_t>, std::string>> repositoryConnectionPool;

    void connEnvironmentClient(std::shared_ptr<rdt::RDTSocket> socket);
    void connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket> socket);
    void manageLinkDetachCommands(const uint8_t& commandKey, std::list<tool::IdNetNode> commandBody, 
                                  const uint16_t& port1, const uint16_t& port2, 
                                  const std::string& remoteIp, tool::IdNetNode id);
    void connEnvironmentLinkIntent(std::shared_ptr<rdt::RDTSocket> socket, const std::string& prompt);

    void runRepositoryListener();

    std::mutex alternateConsolePrintMutex, repoInfoMapMutex, repoCountMutex;
    uint64_t onlineRepositoriesCount;
  public:
    ServerMaster(const uint16_t& listenerPortClient, const uint16_t& listenerPortRepository);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_