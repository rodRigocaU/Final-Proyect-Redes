#ifndef INTERFACE_PERFORMANCE_HPP_
#define INTERFACE_PERFORMANCE_HPP_

#include <string>
#include <sstream>

#include "Colors.hpp"

namespace tool
{

  const std::string getInput(const char* message, const char* COLOR_){
    ConsolePrint(message, COLOR_);
    std::string obj; 
    std::getline(std::cin, obj);
    return obj;
  }

}

#endif//INTERFACE_PERFORMANCE_HPP_