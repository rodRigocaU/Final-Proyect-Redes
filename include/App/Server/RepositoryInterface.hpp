#ifndef REPOSITORY_INTERFACE_HPP_
#define REPOSITORY_INTERFACE_HPP_

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "../../Network/RDTSocket.hpp"
#include "../../Network/RDTListener.hpp"
#include "Repository0MasterProtocol.hpp"
#include "../../DataBase/Sqlite.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"

namespace app{

  class RepositoryServer{
  private:
    std::map<tool::IdNetNode, std::pair<std::pair<uint16_t, uint16_t>, std::string>> neighbours;
    rdt::RDTSocket masterServerSocket;
    rdt::RDTListener unknownLinkListener, unknownQueryListener;
    tool::IdNetNode ownId;
    db::SQLite database;

    std::mutex neighboursMapMutex;
    void connEnvironmentLink(std::shared_ptr<rdt::RDTSocket> socket);
    void connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket> socket);

    void runLinkListener();
    void runQueryListener();
    void detachNeighbours(const std::string& nList);
    void propagateReadable(std::shared_ptr<rdt::RDTSocket> socket, const std::string& readPacket, std::queue<std::string>& taskQueue);
  public:
    RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort, const std::string& databaseFile);
    ~RepositoryServer();
    void run();
  };

}

#endif//REPOSITORY_INTERFACE_HPP_