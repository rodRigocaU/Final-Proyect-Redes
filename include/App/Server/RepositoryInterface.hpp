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
#include "../../DataBase/Sqlite.hpp"
#include "App/TransportParser/Client0MainServerParser.hpp"
#include "../Tools/InterfacePerformance.hpp"
#include "../Tools/Colors.hpp"

namespace app{

  class RepositoryServer{
  private:
    rdt::RDTSocket masterServerSocket;
    rdt::RDTListener unknownQueryListener;
    db::SQLite database;

    std::mutex alternateConsolePrintMutex, databaseMutex;
    void connEnvironmentQuery(std::shared_ptr<rdt::RDTSocket> socket);
  public:
    RepositoryServer(const std::string& serverMasterIp, const uint16_t& serverMasterPort, const std::string& databaseFile);
    ~RepositoryServer();
    void run();
  };

}

#endif//REPOSITORY_INTERFACE_HPP_