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

  std::ifstream file_words(argv[1]);

  if (!file_words)
  {
    std::cout << "Invalid file:" << argv[1] << std::endl;
    return 0;
  }

  std::ofstream file_dict(argv[2]);

  if (!file_dict)
  {
    std::cout << "Invalid file:" << argv[2] << std::endl;
    return 0;
  }

  PTrie ptrie;
  unsigned long i;
  std::string str;
  while (!file_words.eof())
  {
    file_words >> str;
    file_words >> i;
    
    ptrie.insert(str, i);
  }
  file_words.close();

  ptrie.sort();

  ptrie.serialize(file_dict);

  file_dict.close();

  return 0;
}

