cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
  message( FATAL_ERROR "Project \'CENAPSE\' can run only in Linux or Unix." )
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(Cenapse)#Means: "Synapse", but Azure took this name first :,(

include_directories(include)

add_executable(CenapseClient src/Client.cpp)
add_executable(CenapseServer src/MainServer.cpp)
add_executable(CenapseSlave src/Slave.cpp)