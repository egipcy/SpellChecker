#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <fstream>
#include <bitset>

class PTrie
{
private:
  std::vector<std::tuple<std::vector<std::bitset<8>>, std::shared_ptr<PTrie>, unsigned long>> v_;

public:
  enum type {STRING, CHILD, FREQUENCE};

  PTrie();

  void insert(const std::string&, unsigned long);

  void print(int nb_indent = 0) const;

  void sort();

  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search(const std::string& word, unsigned int length);

  void serialize(std::ofstream& file);
  void deserialize(std::ifstream& file);

private:
  size_t search_prefix(const std::string&) const;

  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search0(const std::string& word);
};

void print_result(const std::vector<std::tuple<std::string, unsigned long, unsigned int>>& result);

std::vector<std::bitset<8>> to_bitset(const std::string& s);
std::string to_string(const std::vector<std::bitset<8>>& v);
bool is_equal(std::bitset<8> b, char c);
bool is_equal(char c, std::bitset<8> b);