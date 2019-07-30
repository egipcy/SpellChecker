#pragma once

#include <bitset>
#include <string>
#include <vector>

#define LENGTH_ARRAY 1 << 23//25
#define NB_HASHES 4//6

class BloomFilter
{
public:
  BloomFilter();

  void add(const std::string& word);
  bool has_word(const std::string& word);

  std::string to_string() const;
  void from_string(const std::string& str);

private:
  std::bitset<LENGTH_ARRAY> array_;

  std::vector<uint32_t> calculate(const std::string& word);
};