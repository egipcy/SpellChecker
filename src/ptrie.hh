#include <string>
#include <vector>
#include <memory>
#include <utility>

class PTrie
{
private:
  std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, bool, unsigned long>> v_;

public:
  PTrie();

  void insert(const std::string&, unsigned long);

  void print(int nb_indent = 0) const;

  void sort();

private:
  size_t search_prefix(const std::string&) const;
};