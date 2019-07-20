#include <iostream>
#include <assert.h>
#include <stdio.h>

#include "../src/ptrie.hh"

int main()
{
  std::cout << "\033[1;37;40m" << "SearchN" << "\033[0m" << std::endl;

  std::vector<std::string> v = {
    "zouzou69", "zouzou96", "zouzou9777"
  };

  PTrie ptrie;
  unsigned long frequence = 1;
  for (auto e: v)
    ptrie.insert(e, frequence++);
  ptrie.sort();


  std::vector<unsigned int> lengths = {1};
  std::vector<std::string> words = {"zouzou69"};

  for (auto length: lengths)
    for (auto word: words)
    {
      std::cout << "approx " << length << " " << word << std::endl;
      print_result(ptrie.search(word, length));
    }

  std::cout << "OK" << std::endl;
  
  return 0;
}