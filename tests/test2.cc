#include <iostream>
#include <fstream>

#include "../src/ptrie.hh"

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

  unsigned long i;
  std::string str;
  while (!file.eof())
  {
    file >> str;
    file >> i;
    
    ptrie.insert(str, i);
  }
  file.close();

  std::cout << "Sort" << std::endl;

  ptrie.sort();

  std::cout << "END" << std::endl;

  char a;
  std::cin >> a;
  return 0;
}

