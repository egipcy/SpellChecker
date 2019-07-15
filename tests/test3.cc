#include <iostream>

#include "../src/ptrie.hh"

int main()
{
  PTrie pt;
  std::shared_ptr<PTrie> ptrie = std::make_shared<PTrie>(pt);

  std::vector<std::string> v = {
    "navet", "maison", "maisonnette", "maisonnettes", "mai",
    "maitre", "navet", "naviguer", "navette", "nouveau", "machoire",
    "maitresse", "maitrise"
  };

  std::cout << "Base:" << std::endl;
  ptrie->print();

  unsigned long frequence = 1;
  for (auto e : v)
  {
    std::cout << "Insert " << e << " f=" << frequence << std::endl;
    ptrie->insert(e, frequence++);
  }
  ptrie->sort();
  std::vector<std::tuple<std::string, unsigned long, unsigned int>> r =ptrie->search("maison",0);
  std::cout << "Search maison: " << std::endl;
  std::cout << "size: " << r.size() << std::endl;
  std::cout << "freq: " << std::get<1>(r[0]) << std::endl;
  std::cout << "distance: " << std::get<2>(r[0]) << std::endl;

  return 0;
}