#include <iostream>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <sstream>
#include <cstring>
#include <stdlib.h>
#include "ptrie.hh"

PTrie::PTrie()
{ }

PTrie::PTrie(const std::shared_ptr<PTrie>& parent)
  : parent_(parent)
{ }

void PTrie::insert(const std::string& word, unsigned long frequence)
{
  if (word.size() == 0)
    return;

  // Search a prefix of 'word' in edges
  auto index = search_prefix(word);

  // Check index
  if (index == v_.size()) // Word doesn't exist
  {
    // New edge
    v_.emplace_back(word, nullptr, frequence);
    return;
  }

  // else
  // Check word
  auto& tuple = v_[index];
  auto& w = std::get<STRING>(tuple);

  // word = prefix + suffix1
  // w = prefix + suffix2
  size_t i;
  for (i = 1; i < word.size() && i < w.size() && word[i] == w[i]; i++)
    continue;
  std::string prefix = word.substr(0, i);
  std::string suffix1 = word.substr(i, word.size() - i);
  std::string suffix2 = w.substr(i);

  // No suffixes
  if (suffix1.size() == 0 && suffix2.size() == 0)
    std::get<FREQUENCE>(tuple) = frequence;
  else
  {
    if (suffix2.size() != 0)
    {
      // Edge value set to shared prefix
      w = prefix;

      // Remember the child, the existence value and the frequence
      auto child = std::get<CHILD>(tuple);
      auto freq = std::get<FREQUENCE>(tuple);

      if (suffix1.size() != 0) // w isn't a valid word anymore
        std::get<FREQUENCE>(tuple) = 0;
      else  // Update frequence value
        std::get<FREQUENCE>(tuple) = frequence;

      // New node
      std::get<CHILD>(tuple) = std::make_shared<PTrie>();
      std::get<CHILD>(tuple)->v_.emplace_back(suffix2, child, freq);
    }
    // Insert suffix1 in child
    else if (!std::get<CHILD>(tuple))
      std::get<CHILD>(tuple) = std::make_shared<PTrie>();
    std::get<CHILD>(tuple)->insert(suffix1, frequence);
  }
}

void PTrie::print(int nb_indent) const
{
  for (const auto& e: v_)
  {
    for (int i = 0; i < nb_indent; i++)
      std::cout << ' ';
    std::cout << std::get<STRING>(e) << " f=" << std::get<FREQUENCE>(e) << std::endl;
    if (std::get<CHILD>(e))
      std::get<CHILD>(e)->print(nb_indent + 1);
  }
  if (nb_indent == 0)
    std::cout << std::endl;
}

void PTrie::sort()
{
  // Recursive calls
  for (auto e: v_)
    if (std::get<CHILD>(e))
      std::get<CHILD>(e)->sort();

  // Sort v_'s strings
  std::sort(v_.begin(), v_.end());
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search(const std::string& word, unsigned int length)
{
  return search_rec(word, "", length, length);
}

void PTrie::save_nodes_meta(std::ofstream& file, int depth, int& offset)
{
  char c = ',';
  char sc = ';';
  char n = '\n';
  const std::string ds = std::to_string(depth);
  std::string os;
  for (size_t i = 0; i < v_.size(); i++)
  {
    os = std::to_string(offset);
    const std::string ss = std::to_string(std::get<STRING>(v_[i]).size());
    file.write(ds.c_str(), ds.size());
    file.write(&c, sizeof(c));
    file.write(os.c_str(), os.size());
    file.write(&c, sizeof(c));
    file.write(ss.c_str(), ss.size());
    if (std::get<FREQUENCE>(v_[i]))
    {
      const std::string fs = std::to_string(std::get<FREQUENCE>(v_[i]));
      file.write(&c, sizeof(c));
      file.write(fs.c_str(), fs.size());
    } 
    file.write(&sc, sizeof(sc));
    offset += std::get<STRING>(v_[i]).size();
    if (std::get<CHILD>(v_[i]) != nullptr)
      std::get<CHILD>(v_[i])->save_nodes_meta(file, depth + 1, offset);
  }
}

void PTrie::save_edges(std::ofstream& file)
{
  for (size_t i = 0; i < v_.size(); i++)
  {
    file.write(std::get<STRING>(v_[i]).c_str(), std::get<STRING>(v_[i]).size());
    if (std::get<CHILD>(v_[i]) != nullptr)
      std::get<CHILD>(v_[i])->save_edges(file);
  }
}


/**
 * file's pattern: 
 *  meta data (depth,offset,count[,freq];)
 *  ;
 *  data (strings)
 *  \n
 *  data starting offset (number)
 */
void PTrie::serialize(std::ofstream& file)
{
  int depth = 0;
  int offset = 0;
  save_nodes_meta(file, depth, offset);
  std::string pos = std::to_string(file.tellp()); //Offset to the data
  save_edges(file);
  char n = '\n';
  file.write(&n, sizeof(n)); //Delemiter between data and offset
  file.write(pos.c_str(), pos.size()); //Append the data offset at the end
}

void PTrie::deserialize(const char* file_name)
{
  int fd = ::open(file_name, O_RDONLY);
  if (fd < 0)
  {
    std::cerr << "Error opening " << file_name << std::endl;
    exit(1);
  }
  auto file_size = lseek(fd, 0, SEEK_END);
  char *chunk = reinterpret_cast<char*>(mmap(NULL, file_size, PROT_READ, MAP_FILE | MAP_PRIVATE | MAP_POPULATE, fd, 0));
  ::close(fd);

  //Read the numbers at the end of the file
  int i = 0;
  while (*(chunk+file_size+(--i)) != '\n')
  {
    continue;
  }
  int data_start = atoi(chunk+file_size+i+1); //at *(chunk+dataStart) starts the data
  build_node(0, 0, 2, chunk, data_start, file_size);
}

/** Constructs nodes from dict
 * dict follows this pattern: depth,offset,count[,freq];
 * depth: depth of the node
 * last_depth: this functions starts reading at offset. last_depth is the depth that has been
 *  read before calling this function
 * curr_pos: cursor position while reading meta datas to construct the nodes
 * 
 * Function's steps:
 * -Reads the pattern from offset to ";"
 * -Saves as child in v2_
 * -Reads the next depth
 *  -if read depth == depth: Continues in the while loop
 *  -if read depth < depth: Calls the parent node
 *  -if read depth > depth: Creates and stores a PTrie in the last element of v2_,
 *    then calls build_node on it
 */
void PTrie::build_node(int depth, int last_depth, int curr_pos, const char* chunk, int data_start, const off_t& file_size)
{
  const char* data_chunk = chunk + data_start;
  std::shared_ptr<PTrie> pt(this);
  int current_depth = depth;
  int read_depth = last_depth;
  while (true)
  {
    if (read_depth < current_depth)
    {
      current_depth--;
      pt = pt->parent_;
      continue;
    } 

    //They will be later assign to numerical variables
    int dep = read_depth;
    int of,co;
    unsigned long fr = 0;

    while (curr_pos < data_start)
    {
      //offset
      of = atoi(chunk+curr_pos++);
      next_comma(curr_pos, chunk);

      co = atoi(chunk+curr_pos);

      while (*(chunk+curr_pos++) != ';')
      {
        if (*(chunk+curr_pos) == ',')
        {
          fr = strtoul(chunk+(++curr_pos), nullptr, 10);
        }
      }
      pt->v_.emplace_back(std::string(data_chunk + of, co), nullptr, fr);
      if (curr_pos >= data_start-1)
      {
        return;
      }
      
      //Reading next depth
      read_depth = atoi(chunk+curr_pos++);
      next_comma(curr_pos, chunk);

      //if same dep than current node depth continue in the while
      if (read_depth == current_depth)
      {
        fr = 0;
        continue;
      }
      else
        break;
    }

    if (read_depth < current_depth)
    {
      current_depth--;
      pt = pt->parent_;
      continue;
    }

    //else
    std::shared_ptr<PTrie> new_p = std::make_shared<PTrie>(pt);
    std::get<CHILD>(pt->v_[pt->v_.size()-1]) = new_p;
    current_depth++;
    pt = new_p;
  }
}

/**
 * Moves currsor after the next "," to read with atoi
 */
void PTrie::next_comma(int& curr_pos, const char* chunk)
{
  while (*(chunk+curr_pos++) != ',')
    continue;
}


size_t PTrie::search_prefix(const std::string& word) const
{
  size_t i;
  for (i = 0; i < v_.size(); i++)
    if (std::get<STRING>(v_[i])[0] == word[0])
      break;
  return i;
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search_rec(const std::string& word, const std::string& prefix_w, unsigned int length, unsigned int origin_length)
{
  if (length == 0)
    return search0(word, prefix_w, origin_length);

  return searchN(word, prefix_w, length, origin_length);
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search0(const std::string& word, const std::string& prefix_w, unsigned int origin_length)
{
  for (const auto& e: v_)
  {
    std::string w = std::get<STRING>(e);

    if (w.size() <= word.size() && memcmp(word.c_str(), w.c_str(), w.size()) == 0)
    {
      if (w.size() == word.size())
        return {std::tuple<std::string, unsigned long, unsigned int>(prefix_w + w, std::get<FREQUENCE>(e), origin_length)};

      auto child = std::get<CHILD>(e);
      if (child)
        return child->search0(word.substr(w.size()), prefix_w + w, origin_length);
    }
  }

  return {};
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::searchN(const std::string& word, const std::string& prefix_w, unsigned int length, unsigned int origin_length)
{
  std::vector<std::tuple<std::string, unsigned long, unsigned int>> ret;

  for (const auto& e: v_)
  {
    std::string w = std::get<STRING>(e);
    auto freq = std::get<FREQUENCE>(e);
    auto child = std::get<CHILD>(e);

    auto prefix_word = word.substr(0, w.size());

    unsigned int l;
    bool b;
    std::tie(l, b) = damereau_levenshtein(w, prefix_word, word, length, freq); // DL between w and prefix_word and between w and word

    if (b) // w is accepted
      ret.emplace_back(prefix_w + w, freq, origin_length);

    if (l <= length && child) // our word is ok
    {
      auto v = child->search_rec(word.size() <= w.size() ? "" : word.substr(w.size()), prefix_w + w, length - l, origin_length);

      // ret = ret + v
      ret.reserve(ret.size() + v.size());
      ret.insert(ret.end(), v.begin(), v.end());
    }
  }

  return ret;
}

void print_result(const std::vector<std::tuple<std::string, unsigned long, unsigned int>>& result)
{
  std::cout << "[";

  for (size_t i = 0; i < result.size(); i++)
  {
    auto e = result[i];
    
    std::cout << "{\"word\":\"" << std::get<0>(e) << "\",\"freq\":" << std::get<1>(e)
      << ",\"distance\":" << std::get<2>(e) << "}";

    if (i < result.size() - 1)
      std::cout << ",";
  }

  std::cout << "]" << std::endl;
}

std::tuple<unsigned int, bool>
damereau_levenshtein(const std::string& w, const std::string& prefix_word, const std::string& word, unsigned int length, unsigned long freq)
{
  // 4 operations: insertion, deletion, edition, inversion
  // No need to exceed length in term of distance
  // return <X, true> if freq != 0 and w is accepted
  // word = prefix_word + ...

  auto word_used = freq == 0 ? prefix_word : word;

  std::vector<std::vector<unsigned int>> d(w.size() + 1);
  for (size_t i = 0; i <= w.size(); i++)
  {
    d[i] = std::vector<unsigned int>(word_used.size() + 1);
    d[i][0] = i;
  }
  for (size_t j = 1; j <= word_used.size(); j++)
    d[0][j] = j;

  unsigned int sub_or_exact = 0;
  bool b = freq != 0;
  bool b_break = false;
  for (size_t i = 1; i <= w.size(); i++)
    for (size_t j = 1; j <= word_used.size(); j++)
    {
      sub_or_exact = w[i - 1] == word_used[j - 1] ? 0 : 1;

      d[i][j] = std::min({
        d[i - 1][j] + 1,
        d[i][j - 1] + 1,
        d[i - 1][j - 1] + sub_or_exact});

      if (i > 1 && j > 1 && w[i - 1] == word_used[j - 2] && w[i - 2] == word_used[j - 1])
        d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + sub_or_exact);

      if (d[i][j] > length)
      {
        unsigned int min = d[i][0];
        for (unsigned int k = 1; k < j && min > length; k++)
          min = std::min(min, d[i][k]);
        for (auto it = d[i - 1].cbegin() + j - 1; it < d[i - 1].cend() && min > length; it++)
          min = std::min(min, *it);

        if (min > length)
        {
          b = false;
          b_break = true;
          i = w.size() + 1;
          break;
        }
      }
    }

  /*std::cout << "  ";
  for (size_t j = 1; j <= word_used.size(); j++)
    std::cout << word_used[j - 1] << " ";
  std::cout << std::endl;
  for (size_t i = 1; i <= w.size(); i++)
  {
    std::cout << w[i - 1] << " ";
    for (size_t j = 1; j <= word_used.size(); j++)
      std::cout << d[i][j] << " ";
    std::cout << std::endl;
  }*/

  return std::tuple(b_break ? length + 1 : d[w.size()][prefix_word.size()], b && d[w.size()][word.size()] <= length);
} 