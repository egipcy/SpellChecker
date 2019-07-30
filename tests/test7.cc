#include <iostream>
#include <cassert>

#include "../src/bloom.hh"

int main()
{
  BloomFilter bf;

  bf.add("42");
  bf.add("is the answer");

  assert(bf.has_word("42"));
  assert(bf.has_word("is the answer"));
  assert(!bf.has_word("43"));

  std::cout << "ok" << std::endl;
  auto s = bf.to_string();
  std::cout << "ok" << std::endl;
  bf.from_string(s);
  std::cout << "ok" << std::endl;

  assert(bf.to_string() == s);
  assert(bf.has_word("42"));
  assert(bf.has_word("is the answer"));
  assert(!bf.has_word("43"));

  return 0;
}