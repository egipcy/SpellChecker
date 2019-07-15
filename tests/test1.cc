#include <iostream>

#include "../src/ptrie.hh"

int main()
{
  PTrie ptrie;

  std::vector<std::string> v = {
    "navet", "maison", "maisonnette", "maisonnettes", "mai",
    "maitre", "navet", "naviguer", "navette", "nouveau", "machoire",
    "maitresse", "maitrise"
  };

  std::cout << "Base:" << std::endl;
  ptrie.print();

  unsigned long frequence = 1;
  for (auto e : v)
  {
    std::cout << "Insert " << e << " f=" << frequence << std::endl;
    ptrie.insert(e, frequence++);
    ptrie.print();
  }

  std::cout << "Sort" << std::endl;
  ptrie.sort();
  ptrie.print();

  std::ofstream f1("file.txt");
  ptrie.serialize(f1);

  PTrie ptrie2;
  std::ifstream f2("file.txt");
  ptrie2.deserialize(f2);
  ptrie2.print();

  return 0;
}