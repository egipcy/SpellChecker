#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <fstream>
#include <set>

#include "bloom.hh"

#define BLOOM_FILTER false

class PTrie
{
private:
  std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, unsigned long>> v_;

  std::shared_ptr<PTrie> parent_;

public:
  static inline std::string vocabulary_;

#if BLOOM_FILTER
  static inline BloomFilter filter_;
#endif

public:
  enum type {STRING, CHILD, FREQUENCE};

  PTrie();
  PTrie(const std::shared_ptr<PTrie>& parent);

  void insert(const std::string&, unsigned long);

  void print(int nb_indent = 0) const;
  void print_compressed(int depth) const;

  void sort();

  void serialize(std::ofstream& file);
  void deserialize(const char* file_name);

private:
  void save_nodes_meta(std::ofstream& file, int depth, int& offset);
  void save_edges(std::ofstream& file);
  size_t search_prefix(const std::string&) const;
  void build_node(int depth, int last_depth, int curr_pos, const char* chunk, int data_start, const off_t& file_size);
  void next_comma(int& curr_pos, const char* chunk);

public:
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search(const std::string& word, unsigned int length);

private:
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search0(const std::string& word, const std::string& prefix_w, unsigned int origin_length);
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search0_rec(const std::string& word, const std::string& prefix_w, unsigned int origin_length);
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search1(const std::string& word, unsigned int origin_length);
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  searchN(const std::vector<std::vector<unsigned int>>& d, const std::string& word, const std::string& prefix_w, unsigned int origin_length);
};

void print_result(const std::vector<std::tuple<std::string, unsigned long, unsigned int>>& result);

std::vector<std::vector<unsigned int>>
damerau_levenshtein(std::vector<std::vector<unsigned int>> dl, const std::string& w, const char lastchar_w, const std::string& word, unsigned int length);

size_t dicho(const std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, unsigned long>>& v, const std::string& word);

template <typename T>
void extend(std::vector<T>& ret, const std::vector<T>& v);