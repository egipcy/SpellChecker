#include <iostream>
#include <fstream>

#include "ptrie.hh"

int main(int argc, char** argv)
{
  PTrie ptrie;

  if (argc != 2)
  {
    std::cout << "Usage: ./bin /path/to/words.txt" << std::endl;
    return 0;
  }

  std::ifstream file(argv[1]);

  if (!file)
  {
    std::cout << "Invalid file" << std::endl;
    return 0;
  }

  unsigned short i;
  std::string str;
  while (!file.eof())
  {
    file >> str;
    file >> i;

    ptrie.insert(str, i);
  }


  std::cout << "ok" << std::endl;

  char a;
  std::cin >> a;
  return 0;
}
