#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <fstream>

class PTrie
{
private:
  std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, unsigned long>> v_;
  std::vector<std::tuple<int, int, std::shared_ptr<PTrie>, unsigned long>> v2_;
  PTrie* parent_;
  int file_size_;
  char* chunk_;
  int data_start_;
  char* data_chunk_;

public:
  enum type {STRING, CHILD, FREQUENCE};
  enum G {OFFSET, COUNT, SON, FREQ};

  PTrie();
  PTrie(PTrie* parent, int file_size, char* chunk, int data_start);
  void insert(const std::string&, unsigned long);

  void print(int nb_indent = 0) const;
  void print_compressed(int depth) const;

  void sort();

  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search(const std::string& word, unsigned int length);

  void serialize(std::ofstream& file);
  void deserialize(const char* file_name);

private:
  void save_nodes_meta(std::ofstream& file, int depth, int& offset);
  void save_edges(std::ofstream& file);
  size_t search_prefix(const std::string&) const;
  void build_compressed_trie(char* chunk, int data_start, int file_size);
  void build_node(int depth, int last_depth, int& curr_pos);
  void next_comma(int& curr_pos);
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search_rec(const std::string& word, const std::string& prefix_w, unsigned int length, unsigned int origin_length);
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  search0(const std::string& word);
  
public:
  std::vector<std::tuple<std::string, unsigned long, unsigned int>>
  searchN(const std::string& word, const std::string& prefix_w, unsigned int length, unsigned int origin_length);
};

void print_result(const std::vector<std::tuple<std::string, unsigned long, unsigned int>>& result);

std::tuple<unsigned int, bool>
damereau_levenshtein(const std::string& w, const std::string& prefix_word, const std::string& word, unsigned int length, unsigned long freq);