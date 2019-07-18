#include <iostream>
#include <assert.h>
#include "../src/ptrie.hh"

int main()
{
  std::cout << "\033[1;37;40m" << "Binary serialization" << "\033[0m" << std::endl;
  PTrie ptrie;

  std::vector<std::string> v = {
    "navet", "maison", "maisonnette", "maisonnettes", "mai",
    "maitre", "navet", "naviguer", "navette", "nouveau", "machoire",
    "maitresse", "maitrise"
  };

  unsigned long frequence = 1;
  for (auto e : v)
  {
    ptrie.insert(e, frequence++);
  }
  ptrie.sort();
  std::ofstream f1("mydict.bin", std::ofstream::binary);
  ptrie.serialize(f1);
  f1.close();
  ptrie.deserialize("mydict.bin");
  
  return 0;
}