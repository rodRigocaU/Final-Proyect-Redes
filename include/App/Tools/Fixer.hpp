#ifndef FIXER_HPP_
#define FIXER_HPP_

#include <iostream>
#include <string>

#include "Colors.hpp"

namespace tool
{

  const std::string fixToBytes(const std::string &input, const std::size_t& n_bytes);
  
  const std::string asStreamString(std::string &source, const std::size_t& h_bytes);

  const std::size_t asStreamNumeric(std::string &source, const std::size_t& h_bytes);

  void paddingPacket(std::string& message, const char& item, const std::size_t& bytes);
}

#endif//