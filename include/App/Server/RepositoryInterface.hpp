#ifndef REPOSITORY_INTERFACE_HPP_
#define REPOSITORY_INTERFACE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"

namespace app{

  class RepositoryServer{
  private:
    void linkRepositoryNeighbour();

    std::vector<std::shared_ptr<rdt::RDTSocket>> neighbours;
    rdt::RDTSocket masterServerSocket;
    rdt::RDTListener unknownListener;
  public:
    RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort);
    ~RepositoryServer();
    void run();
  };

}

#endif//REPOSITORY_INTERFACE_HPP_