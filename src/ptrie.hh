#include <string>
#include <vector>
#include <memory>
#include <utility>

class PTrie
{
private:
  std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, bool, unsigned short>> v_;

public:
  PTrie();

  void insert(const std::string&, unsigned short);

  void print(int nb_indent = 0) const;

  std::vector<std::pair<std::string, unsigned short>> search(const std::string&, unsigned short) const;

private:
  size_t search_prefix(const std::string&) const;

  std::pair<std::string, unsigned short> search_distance0(const std::string&) const;
};