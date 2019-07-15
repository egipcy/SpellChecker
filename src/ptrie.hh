#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <fstream>

class PTrie
{
private:
  std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, bool, unsigned long>> v_;

public:
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