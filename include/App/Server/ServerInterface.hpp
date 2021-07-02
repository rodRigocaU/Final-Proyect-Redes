#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"

namespace app{

  class ServerMaster {
  private:
    typedef uint64_t IdNetNode;
    typedef int32_t FileDescriptor;

    rdt::RDTListener clientListener, repositoryListener;
    std::map<IdNetNode, std::shared_ptr<rdt::RDTSocket>> repositoryConnectionPool;
    std::map<FileDescriptor, std::shared_ptr<rdt::RDTSocket>> clientConnectionPool;

    void connEnvironmentClient(std::shared_ptr<rdt::RDTSocket>& socket);
    void connEnvironmentRepository(std::shared_ptr<rdt::RDTSocket>& socket);

    void runRepositoryListener();
  public:
    ServerMaster(const uint16_t& listenerPortClient, const uint16_t& listenerPortRepository);
    void run();
  };

}




#endif//SERVER_INTERFACE_HPP_