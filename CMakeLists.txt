cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(WIN32)
  message( FATAL_ERROR "Project \'CENAPSE\' can run only in Linux or Unix." )
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(Cenapse)#Means: "Synapse", but Azure took this name first :cry:

include_directories(include)

add_executable(CenapseClient src/Client.cpp)
add_executable(CenapseServer src/MainServer.cpp)
add_executable(CenapseSlave src/Slave.cpp)

include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(CenapseSlave sqlite3)

# Optional
#find_package (SQLite3)
#if (SQLITE3_FOUND)
#  include_directories(${SQLITE3_INCLUDE_DIRS})
#  target_link_libraries (CenapseSlave ${SQLITE3_LIBRARIES})
#endif (SQLITE3_FOUND)
