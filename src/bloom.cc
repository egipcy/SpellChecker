#include "bloom.hh"

#include <iostream>
#include <cassert>

BloomFilter::BloomFilter()
{ }

void BloomFilter::add(const std::string& word)
{
  auto hashes = calculate(word);
  for (auto h: hashes)
    array_.set(h % array_.size());
}

bool BloomFilter::has_word(const std::string& word)
{
  auto hashes = calculate(word);
  for (auto h: hashes)
    if (!array_[h % array_.size()])
      return false;

  return true;
}

std::string BloomFilter::to_string() const
{
  std::string ret;

  for (size_t i = 0; i < array_.size(); i += 32)
  {
    uint32_t n = 0;
    for (size_t k = 0; k < 32; k++)
      n += array_[i + k] ? 1 << k : 0;
    ret += std::to_string(n) + " ";
  }

  return ret;
}

void BloomFilter::from_string(const std::string& str)
{
  size_t size = 0;
  for (size_t i = 0; i < array_.size(); i += 32)
  {
    size_t s = 0;
    uint32_t n = 0;
    for (s = 0; s < 10; s++)
      if (str[size + s] == ' ')
        break;
      else
        n = n * 10 + (str[size + s] - '0');
    size += s + 1;

    for (size_t k = 0; k < 32; k++)
      array_.set(i + k, (n >> k) % 2 == 1);
  }
}

static uint32_t murmur3(const std::string& word)
{
  uint32_t hash = 0;

  static const uint32_t c1 = 0xcc9e2d51;
  static const uint32_t c2 = 0x1b873593;
  static const uint32_t r1 = 15;
  static const uint32_t r2 = 13;
  static const uint32_t m = 5;
  static const uint32_t n = 0xe6546b64;

  const size_t nblocks = word.size() / 4;
  const uint32_t* blocks = (const uint32_t*) word.c_str();
  for (size_t i = 0; i < nblocks; i++)
  {
    uint32_t k = blocks[i];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;

    hash ^= k;
    hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
  }

  const uint8_t* tail = (const uint8_t*)(word.c_str() + nblocks * 4);
  uint32_t k1 = 0;

  switch (word.size() & 3)
  {
  case 3:
    k1 ^= tail[2] << 16;
  case 2:
    k1 ^= tail[1] << 8;
  case 1:
    k1 ^= tail[0];

    k1 *= c1;
    k1 = (k1 << r1) | (k1 >> (32 - r1));
    k1 *= c2;
    hash ^= k1;
  }

  hash ^= word.size();
  hash ^= (hash >> 16);
  hash *= 0x85ebca6b;
  hash ^= (hash >> 13);
  hash *= 0xc2b2ae35;
  hash ^= (hash >> 16);

  return hash;
}

static uint32_t jenkins(const std::string& word)
{
  uint32_t hash = 0;
  for (size_t i = 0; i < word.size(); i++)
  {
    hash += word[i];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

std::vector<uint32_t> BloomFilter::calculate(const std::string& word)
{
  std::vector<uint32_t> ret(NB_HASHES);

  ret[0] = murmur3(word);
  ret[1] = jenkins(word);

  for (size_t i = 2; i < ret.size(); i++)
    ret[i] = ret[0] + i * ret[1];

  return ret;
}
