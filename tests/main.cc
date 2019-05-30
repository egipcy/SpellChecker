#include <iostream>
#include "../src/ptrie.hh"

int main()
{
  PTrie ptrie;

  std::vector<std::string> v = {
    "maison", "navet", "maisonnette", "maisonnettes", "mai",
    "maitre", "navet", "navette", "naviguer", "nouveau"
  };

  std::cout << "Base:" << std::endl;
  ptrie.print();

  unsigned short frequence = 1;
  for (auto e : v)
  {
    std::cout << "Insert " << e << " f=" << frequence << std::endl;
    ptrie.insert(e, frequence++);
    ptrie.print();
  }

  return 0;
}