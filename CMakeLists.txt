cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  message( FATAL_ERROR "Project \'CENAPSE\' can run only in Linux or Unix." )
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(Cenapse)#Means: "Synapse", but Azure took this name first :cry:

include_directories(include)

file(GLOB SOURCE_FILES
  "src/App/Client/*.cpp"
  "src/App/Server/*.cpp"
  "src/App/Tools/*.cpp"
  "src/App/TransportParser/*.cpp"
  "src/Network/*.cpp"
  "src/Network/Algorithm/*.cpp"
  "src/Database/*.cpp"
)

add_executable(CenapseClient src/Client.cpp ${SOURCE_FILES})
add_executable(CenapseServer src/MainServer.cpp ${SOURCE_FILES})
add_executable(CenapseSlave src/Slave.cpp ${SOURCE_FILES})

include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(CenapseSlave sqlite3)
