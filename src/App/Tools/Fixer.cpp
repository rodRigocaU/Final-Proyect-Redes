#include "App/Tools/Fixer.hpp"

namespace tool{

  const std::string fixToBytes(const std::string &input, const std::size_t& n_bytes) {
    if(input.length() > n_bytes) {
      ConsolePrint("[Error]: The given string is longer than the given number of bytes.", RED);
      exit(EXIT_FAILURE);
    }
    return std::string().assign(n_bytes - input.length(), '0') + input;
  }

  const std::string asStreamString(std::string &source, const std::size_t& h_bytes) {
    std::string tempSrc = source, contentSizeStr = source.substr(0, h_bytes);
    for(auto& digit : contentSizeStr)
      if(!isdigit(digit))
        exit(EXIT_FAILURE);
    std::size_t cSize = std::stoul(contentSizeStr);
    source = source.substr(h_bytes + cSize);
    return tempSrc.substr(h_bytes, cSize);
  }

  const std::size_t asStreamNumeric(std::string &source, const std::size_t& h_bytes) {
    std::string tempSrc = source, contentSizeStr = source.substr(0, h_bytes);
    for(auto& digit : contentSizeStr)
      if(!isdigit(digit))
        exit(EXIT_FAILURE);
    source = source.substr(h_bytes);
    return std::stoul(contentSizeStr);
  }

  void paddingPacket(std::string& message, const char& item, const std::size_t& bytes){
    if(bytes > message.length())
      message += std::string().assign(bytes - message.length(), item);
  }

}