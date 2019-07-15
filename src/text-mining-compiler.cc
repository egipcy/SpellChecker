#include <iostream>
#include <fstream>

#include "ptrie.hh"

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cout << "Usage: ./TextMiningCompiler /path/to/words.txt /path/to/dict.bin" << std::endl;
    return 0;
  }

  std::ifstream file(argv[1]);

  if (!file)
  {
    std::cout << "Invalid file:" << argv[1] << std::endl;
    return 0;
  }

  PTrie ptrie;
  unsigned long i;
  std::string str;
  while (!file.eof())
  {
    file >> str;
    file >> i;
    
    ptrie.insert(str, i);
  }
  file.close();

  ptrie.sort();

  ptrie.serialize(argv[2]);

  return 0;
}

