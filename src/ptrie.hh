#include <string>
#include <vector>
#include <memory>
#include <utility>

class PTrie: public std::enable_shared_from_this<PTrie>
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

  void serialize(const std::string& filename);
  void deserialize(const std::string& filename);

private:
  size_t search_prefix(const std::string&) const;

  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search0(const std::string& word);

  //Utilisé pour pouvoir stocker this dans la même variable que les ptrie fils.
  //https://en.cppreference.com/w/cpp/memory/enable_shared_from_this
public:
  std::shared_ptr<PTrie> get_ptr();
};

void print_result(const std::vector<std::tuple<std::string, unsigned long, unsigned int>>& result);