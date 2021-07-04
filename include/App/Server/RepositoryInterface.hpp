#ifndef REPOSITORY_INTERFACE_HPP_
#define REPOSITORY_INTERFACE_HPP_

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"

namespace app{

  class RepositoryServer{
  private:
    typedef int32_t FileDescriptor;

    void linkRepositoryNeighbour();

    std::vector<std::shared_ptr<rdt::RDTSocket>> neighbours;
    rdt::RDTSocket masterServerSocket;
    rdt::RDTListener unknownLinkListener, unknownQueryListener;

    std::map<FileDescriptor, std::shared_ptr<rdt::RDTSocket>> neighbourConnectionPool, queryConnectionPool;
    std::mutex alternateConsolePrintMutex;
    void connEnvironmentLink(std::shared_ptr<rdt::RDTSocket>& socket);
    void connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket>& socket);

    void runLinkListener();
    void runQueryListener();
  public:
    RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort);
    ~RepositoryServer();
    void run();
  };

}

#endif//REPOSITORY_INTERFACE_HPP_