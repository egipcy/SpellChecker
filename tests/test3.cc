#include <iostream>
#include <assert.h>
#include "../src/ptrie.hh"

int main()
{
  std::cout << "\033[1;37;40m" << "Search (d=0)" << "\033[0m" << std::endl;
  PTrie ptrie;
  ptrie.deserialize("test_dict.bin");
  std::vector<std::tuple<std::string, unsigned long, unsigned int>> r = ptrie.search("maison",0);
  assert(r.size() == 1);
  assert(std::get<1>(r[0]) == 2);
  assert(std::get<2>(r[0]) == 0);
  std::cout << "\033[32m" << "maison" << "\033[0m" << std::endl;

  std::vector<std::tuple<std::string, unsigned long, unsigned int>> r2 = ptrie.search("maiso",0);
  assert(r2.size() == 0);
  std::cout << "\033[32m" << "maiso" << "\033[0m" << std::endl;

  std::vector<std::tuple<std::string, unsigned long, unsigned int>> r3 = ptrie.search("maisonn",0);
  assert(r3.size() == 0);
  std::cout << "\033[32m" << "maisonn" << "\033[0m" << std::endl;

  std::vector<std::tuple<std::string, unsigned long, unsigned int>> r4 = ptrie.search("Maison",0);
  assert(r4.size() == 0);
  std::cout << "\033[32m" << "Maison" << "\033[0m" << std::endl;
  
  return 0;
}