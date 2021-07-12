#ifndef INTERFACE_PERFORMANCE_HPP_
#define INTERFACE_PERFORMANCE_HPP_

#include <fstream>
#include <map>
#include <string>
#include <sstream>

#include "Colors.hpp"

#define DOT_CONF_TOKEN_SEPARATOR    '='
#define DOT_CONF_TOKEN_COMMENT      '#'

namespace tool
{

  const std::string getInput(const char* message, const char* COLOR_);

  void cleanSpaces(std::string& word);

  bool readSettingsFile(const char* filePath, std::map<std::string, std::string>& storage, bool cleanQuots = false);
}

#endif//INTERFACE_PERFORMANCE_HPP_