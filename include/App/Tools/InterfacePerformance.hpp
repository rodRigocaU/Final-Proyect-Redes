#ifndef INTERFACE_PERFORMANCE_HPP_
#define INTERFACE_PERFORMANCE_HPP_

#include <fstream>
#include <map>
#include <string>
#include <sstream>

#include "Colors.hpp"

namespace tool
{

  const std::string getInput(const char* message, const char* COLOR_);

  void cleanSpaces(std::string& word);

  bool readSettingsFile(const char* filePath, std::map<std::string, std::string>& storage);
}

#endif//INTERFACE_PERFORMANCE_HPP_