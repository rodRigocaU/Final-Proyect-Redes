#ifndef REPOSITORY_INTERFACE_HPP_
#define REPOSITORY_INTERFACE_HPP_

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"
#include "Repository0MasterProtocol.hpp"

namespace app{

  class RepositoryServer{
  private:
    std::map<tool::IdNetNode, std::pair<std::pair<uint16_t, uint16_t>, std::string>> neighbours;
    rdt::RDTSocket masterServerSocket;
    rdt::RDTListener unknownLinkListener, unknownQueryListener;
    tool::IdNetNode ownId;

    std::mutex alternateConsolePrintMutex, neighboursMapMutex;
    void connEnvironmentLink(std::shared_ptr<rdt::RDTSocket> socket);
    void connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket> socket);

    void runLinkListener();
    void runQueryListener();
  public:
    RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort);
    ~RepositoryServer();
    void run();
  };

}

#endif//REPOSITORY_INTERFACE_HPP_