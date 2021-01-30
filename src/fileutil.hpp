#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <picosha2.hpp>


namespace util {


inline std::vector<uint8_t> readVectorFromFile(std::string const & filename)
{
  std::ifstream stream(filename, std::ios::in | std::ios::binary);
  std::vector<uint8_t> data;

  std::for_each(std::istreambuf_iterator<char>(stream),
    std::istreambuf_iterator<char>(),
    [&data](const char c){data.push_back(c);});
      
  return data;      
}


inline void writeVectorToFile(std::string const & filenameOutput, std::vector<uint8_t> const & data)
{
  std::ofstream streamOut(filenameOutput, std::ios::out | std::ofstream::binary);
  std::copy(data.cbegin(), data.cend(), std::ostreambuf_iterator<char>(streamOut));
}


inline std::string computeFileHash(std::string const& filename)
{
  std::size_t numBytesUsed = 12;

  std::vector<unsigned char> hashdigest(picosha2::k_digest_size);

  {
    std::ifstream f(filename, std::ios::binary);
    picosha2::hash256(f, hashdigest.begin(), hashdigest.end());
  }

  std::string const hashSha256 = picosha2::bytes_to_hex_string(hashdigest.begin(), hashdigest.end());

  return hashSha256.substr(hashSha256.size() - (2 * numBytesUsed), std::string::npos);
}


} // namespace util
