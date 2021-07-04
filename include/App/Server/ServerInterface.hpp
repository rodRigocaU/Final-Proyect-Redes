#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"
#include "Repository0MasterProtocol.hpp"

#define REPOSITORY_LIMIT   1000

namespace app{

  class ServerMaster {
  private:
    typedef uint64_t IdNetNode;
    typedef int32_t FileDescriptor;

    rdt::RDTListener clientListener, repositoryListener;
    std::map<IdNetNode, std::pair<uint16_t, std::shared_ptr<rdt::RDTSocket>>> repositoryConnectionPool;
    std::map<FileDescriptor, std::shared_ptr<rdt::RDTSocket>> clientConnectionPool;

    void connEnvironmentClient(std::shared_ptr<rdt::RDTSocket>& socket);
    void connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket>& socket);

    void runRepositoryListener();

    std::mutex alternateConsolePrintMutex, repoInteractionMutex, repoTaskQueueMutex;
    uint64_t onlineRepositoriesCount;
    std::queue<std::tuple<char, IdNetNode, IdNetNode>> preProcessRepositoryQueue;
  public:
    ServerMaster(const uint16_t& listenerPortClient, const uint16_t& listenerPortRepository);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_