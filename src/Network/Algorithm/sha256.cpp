#include "Network/Algorithm/sha256.hpp"
#include <array>
#include <iostream>
#include <sstream>
#include <vector>

//Algorithm Idea Link: https://qvault.io/cryptography/how-sha-2-works-step-by-step-sha-256/

const uint32_t roundConstant[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                                    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                                    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                                    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                                    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 
                                    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                                    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                                    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const uint32_t initHashVars[8]   = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

uint32_t choose(const uint32_t& E, const uint32_t& F, const uint32_t& G){
  return (E & F) ^ (~E & G);
}

uint32_t majority(const uint32_t& A, const uint32_t& B, const uint32_t& C){
  return (A & B) ^ (A & C) ^ (B & C);
}

uint32_t leftRotate(uint32_t n, uint32_t d){
  return (n << d)|(n >> (32 - d));
}
 
uint32_t rightRotate(uint32_t n, uint32_t d){
  return (n >> d)|(n << (32 - d));
}

uint32_t sigma0(const uint32_t& A){
  return rightRotate(A, 2) ^ rightRotate(A, 13) ^ rightRotate(A, 22);
}

uint32_t sigma1(const uint32_t& E){
  return rightRotate(E, 6) ^ rightRotate(E, 11) ^ rightRotate(E, 25);
}

uint32_t group32bits(const uint8_t& b0, const uint8_t& b1, const uint8_t& b2, const uint8_t& b3){
  uint32_t word = b0;
  word <<= 8;
  word += b1;
  word <<= 8;
  word += b2;
  word <<= 8;
  word += b3;
  return word;
}

std::vector<uint8_t> sha256Padding(const std::string& message){
  std::vector<uint8_t> fixedInput;
  for(char c : message)
    fixedInput.push_back(c);
  fixedInput.push_back(uint8_t(128));
  std::size_t msgBlockIdx = fixedInput.size();
  uint64_t bigEndianNumber = message.length() * 8;
  for(uint8_t rightShift = 56; rightShift > 0; rightShift -= 8){
    uint64_t tempEndian = bigEndianNumber << (56 - rightShift);
    tempEndian >>= (56 - rightShift);
    fixedInput.push_back(tempEndian >> rightShift);
  }
  uint64_t tempEndian = bigEndianNumber << 56;
  tempEndian >>= 56;
  fixedInput.push_back(tempEndian);
  while(fixedInput.size() % 64){
    fixedInput.insert(fixedInput.begin() + msgBlockIdx, '\0');
  }
  return fixedInput;
}

std::string fixZerosHex(const uint32_t& hn){
  std::stringstream buffer, fixedBuffer;
  buffer << std::hex << hn;
  if(buffer.str().length() < 8){
    fixedBuffer << std::string(8 - buffer.str().length(), '0') << buffer.str();
    return fixedBuffer.str();
  }
  return buffer.str();
}

std::string sha256Concatenate(const uint32_t& h0, const uint32_t& h1, const uint32_t& h2, const uint32_t& h3,
                              const uint32_t& h4, const uint32_t& h5, const uint32_t& h6, const uint32_t& h7){
  std::stringstream buffer;
  buffer << fixZerosHex(h0);
  buffer << fixZerosHex(h1);
  buffer << fixZerosHex(h2);
  buffer << fixZerosHex(h3);
  buffer << fixZerosHex(h4);
  buffer << fixZerosHex(h5);
  buffer << fixZerosHex(h6);
  buffer << fixZerosHex(h7);
  return buffer.str();
}

std::array<uint32_t, 64> makeMessageSchedule(const std::vector<uint8_t>& byteChunk){
  std::array<uint32_t, 64> w;
  w.fill(0);
  for(std::size_t idx = 0, i = 0; i < 16; idx += 4, ++i){
    w[i] = group32bits(byteChunk[idx], byteChunk[idx + 1], byteChunk[idx + 2], byteChunk[idx + 3]);
  }
  for(std::size_t idx = 16; idx < 64; ++idx){
    uint32_t s0 = rightRotate(w[idx - 15], 7) ^ rightRotate(w[idx - 15], 18) ^ (w[idx - 15] >> 3);
    uint32_t s1 = rightRotate(w[idx - 2], 17) ^ rightRotate(w[idx - 2], 19) ^ (w[idx - 2] >> 10);
    w[idx] = w[idx - 16] + s0 + w[idx - 7] + s1;
  }
  return w;
}

std::string crypto::sha256(const std::string& inputMsg){
  std::vector<uint8_t> paddInput = sha256Padding(inputMsg);
  uint32_t h0 = initHashVars[0], h1 = initHashVars[1], h2 = initHashVars[2], h3 = initHashVars[3];
  uint32_t h4 = initHashVars[4], h5 = initHashVars[5], h6 = initHashVars[6], h7 = initHashVars[7];
  for(uint8_t chunk = 0; chunk < paddInput.size(); chunk += 64){
    std::vector<uint8_t> byteChunk(paddInput.begin() + chunk, paddInput.begin() + chunk + 64);
    std::array<uint32_t, 64> W = makeMessageSchedule(byteChunk);
    uint32_t A = h0, B = h1, C = h2, D = h3;
    uint32_t E = h4, F = h5, G = h6, H = h7;
    for(uint8_t i = 0; i < 64; ++i){
      uint32_t temp1 = H + sigma1(E) + choose(E, F, G) + roundConstant[i] + W[i];
      uint32_t temp2 = sigma0(A) + majority(A, B, C);
      H = G;
      G = F;
      F = E;
      E = D + temp1;
      D = C;
      C = B;
      B = A;
      A = temp1 + temp2;
    }
    h0 += A;
    h1 += B;
    h2 += C;
    h3 += D;
    h4 += E;
    h5 += F;
    h6 += G;
    h7 += H;
  }
  return sha256Concatenate(h0, h1, h2, h3, h4, h5, h6, h7);
}