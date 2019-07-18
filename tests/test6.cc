#include <iostream>
#include <assert.h>
#include <stdio.h>

#include "../src/ptrie.hh"

int main()
{
  std::cout << "\033[1;37;40m" << "SearchN" << "\033[0m" << std::endl;

  std::vector<std::string> v = {
    "navet", "maison", "maisonnette", "maisonnettes", "mai",
    "maitre", "navet", "naviguer", "navette", "nouveau", "machoire",
    "maitresse", "maitrise"
  };

  PTrie ptrie;
  unsigned long frequence = 1;
  for (auto e: v)
    ptrie.insert(e, frequence++);
  ptrie.sort();


  std::vector<unsigned int> lengths = {15};
  std::vector<std::string> words = {"m"};

  for (auto length: lengths)
    for (auto word: words)
    {
      std::cout << "approx " << length << " " << word << std::endl;
      print_result(ptrie.searchN(word, "", length, length));
    }

  std::cout << "OK" << std::endl;
  
  return 0;
}