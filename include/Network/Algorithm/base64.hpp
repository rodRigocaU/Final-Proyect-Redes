#ifndef BASE_64_HPP_
#define BASE_64_HPP_

#include <string>

namespace crypto{ 

  std::string encodeBase64(const std::string& contentNormal);

  std::string decodeBase64(const std::string& contentB64);
  
}
#endif//BASE_64_HPP_