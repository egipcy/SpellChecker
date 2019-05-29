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

  for (auto e : v)
  {
    std::cout << "Insert " << e << std::endl;
    ptrie.insert(e);
    ptrie.print();
  }

  return 0;
}