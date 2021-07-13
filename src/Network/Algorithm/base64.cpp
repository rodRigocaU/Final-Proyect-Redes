#include "Network/Algorithm/base64.hpp"

#include <bitset>
#include <iostream>
#include <sstream>

const int8_t base64_chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+";

int32_t getIdxBase64(const int8_t& c){
  for(int32_t letter = 0; letter < 65; ++letter)
    if(c == base64_chars[letter])
      return letter;
  return -1;
}

std::string crypto::encodeBase64(const std::string& contentNormal){
  std::string rawEncodedContent, encodedContent;
  for(int8_t letter : contentNormal){
    rawEncodedContent += std::bitset<8>(int32_t(letter)).to_string();
  }
  while(rawEncodedContent.length() % 6 != 0)
    rawEncodedContent.push_back('0');
  for(std::size_t idx = 0; idx < rawEncodedContent.length(); idx += 6){
    encodedContent.push_back(base64_chars[std::bitset<8>(rawEncodedContent.substr(idx, 6)).to_ulong()]);
  }
  return encodedContent;
}

std::string crypto::decodeBase64(const std::string& contentB64){
  std::string rawDecodedContent, decodedContent;
  for(int8_t letter : contentB64){
    rawDecodedContent += std::bitset<6>(getIdxBase64(letter)).to_string();
  }
  for(std::size_t idx = 0; idx < rawDecodedContent.length(); idx += 8){
    decodedContent += static_cast<int8_t>(std::bitset<8>(rawDecodedContent.substr(idx, 8)).to_ulong());
  }
  return decodedContent;
}