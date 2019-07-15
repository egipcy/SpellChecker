#include <iostream>
#include <fstream>

#include "ptrie.hh"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: ./TextMiningApp /path/to/dict.bin" << std::endl;
    return 0;
  }

  PTrie ptrie;
  ptrie.deserialize(argv[1]);

  std::string line;
  while (std::getline(std::cin, line))
  {
    auto first_space = line.find_first_of(" ");
    auto last_space = line.find_last_of(" ");

    std::string approx = line.substr(0, first_space);
    unsigned int length = std::stoul(line.substr(first_space + 1, last_space - first_space - 1));
    std::string word = line.substr(last_space + 1);

    std::cout << approx << "'" << std::endl << length << "'" << std::endl << word << "'" << std::endl;

    if (approx != "approx")
      continue;

    print_result(ptrie.search(word, length));
  }

  return 0;
}

