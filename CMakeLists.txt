cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  message( FATAL_ERROR "Project \'CENAPSE\' can run only in Linux or Unix." )
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lpthread")

project(Cenapse)#Means: "Synapse", but Azure took this name first :cry:

include_directories(include)

file(GLOB CLIENT_SOURCE_FILES
  "src/App/Client/*.cpp"
  "src/App/Tools/*.cpp"
  "src/App/TransportParser/*.cpp"
  "src/Network/*.cpp"
  "src/Network/Algorithm/*.cpp"
)

file(GLOB MASTER_SOURCE_FILES
  "src/App/Server/ServerInterface.cpp"
  "src/App/Server/Repository0MasterProtocol.cpp"
  "src/App/Tools/*.cpp"
  "src/App/TransportParser/*.cpp"
  "src/Network/*.cpp"
  "src/Network/Algorithm/*.cpp"
)

file(GLOB SLAVE_SOURCE_FILES
  "src/DataBase/*.cpp"
  "src/App/Server/Repository0MasterProtocol.cpp"
  "src/App/Server/RepositoryInterface.cpp"
  "src/App/Tools/*.cpp"
  "src/App/TransportParser/*.cpp"
  "src/Network/*.cpp"
  "src/Network/Algorithm/*.cpp"
)

file(GLOB CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/settings/Cenapse.conf")
file(COPY ${CONFIG_FILES} DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB DB_FILE "${CMAKE_CURRENT_SOURCE_DIR}/GraphNetwork.db")
file(COPY ${DB_FILE} DESTINATION ${CMAKE_BINARY_DIR})

add_executable(CClient src/Client.cpp ${CLIENT_SOURCE_FILES})
add_executable(CServer src/MainServer.cpp ${MASTER_SOURCE_FILES})
add_executable(CSlave src/Slave.cpp ${SLAVE_SOURCE_FILES})

include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(CSlave sqlite3)
