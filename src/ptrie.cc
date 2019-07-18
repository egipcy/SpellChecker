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

PTrie::PTrie(PTrie* parent, int file_size, char* chunk, int data_start):
  parent_(parent)
  ,file_size_(file_size)
  ,chunk_(chunk)
  ,data_start_(data_start)
{}

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

void PTrie::print_compressed(int depth) const
{
  for (auto v: v2_)
  {
    std::cout
      << depth << ","
      << std::get<0>(v) << ","
      << std::get<1>(v);
    if (std::get<3>(v) != 0)
      std:: cout << "," << std::get<3>(v);
    std::cout << ";";
    if (std::get<2>(v) != nullptr)
      std::get<2>(v)->print_compressed(depth+1);
  }
  if (!depth)
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
  if (length == 0)
    return search0(word);

  // TODO

  return std::vector<std::tuple<std::string, unsigned long, unsigned int>>();
}

void PTrie::save_nodes_meta(std::ofstream& file, int depth, int& offset)
{
  char c = ',';
  char sc = ';';
  char n = '\n';
  const std::string ds = std::to_string(depth);
  const std::string os = std::to_string(offset);
  for (size_t i = 0; i < v_.size(); i++)
  {
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

void PTrie::deserialize(std::string file_name)
{
  int fd = ::open(file_name.c_str(), O_RDONLY);
  if (fd < 0)
  {
    std::cerr << "Error opeing " << file_name << std::endl;
    exit(1);
  }
  auto file_size = lseek(fd, 0, SEEK_END);
  char *chunk = reinterpret_cast<char*>(mmap(NULL, file_size, PROT_READ, MAP_FILE | MAP_PRIVATE | MAP_POPULATE, fd, 0));
  ::close(fd);

  std::vector<char> v;
  char c;
  std::stringstream ss;
  int dataStart; //at *(chunk+dataStart) starts the data
  //Read the numbers at the end of the file
  int i = -1;
  while (1)
  {
    c = *(chunk+file_size+(i--));
    if (c != '\n')
    {
      v.emplace_back(c);
    }
    else
    {
      for (auto it = v.rbegin(); it != v.rend(); ++it)
      {
        ss << *it;
      }
      ss >> dataStart;
      break;
    }
  }
  build_compressed_trie(chunk, dataStart, file_size);
}

void PTrie::build_compressed_trie(char* chunk, int data_start, int file_size)
{
  //init root
  chunk_ = chunk;
  data_start_ = data_start;
  file_size_ = file_size;
  parent_ = nullptr;
  int curr_pos = 2; //skipped "0," as build node starts reading at offset

  //buiding nodes
  build_node(0, 0, curr_pos);
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
void PTrie::build_node(int depth, int last_depth, int& curr_pos)
{
  if (last_depth < depth)
    return parent_->build_node(depth - 1, last_depth, curr_pos);

  //They will be later assign to numerical variables
  int dep = last_depth;
  int of,co;
  unsigned long fr = 0;

  while (curr_pos < data_start_)
  {
    //offset
    of = atoi(chunk_+curr_pos++);
    next_comma(curr_pos);
    co = atoi(chunk_+curr_pos);

    while (*(chunk_+curr_pos++) != ';')
    {
      if (*(chunk_+curr_pos) == ',')
      {
        fr = strtoul(chunk_+(++curr_pos), nullptr, 10);
      }
    }
    v2_.emplace_back(of, co, nullptr, fr);
    if (curr_pos >= data_start_-1)
    {
      return;
    }
    
    //Reading next depth
    dep = atoi(chunk_+curr_pos++);
    next_comma(curr_pos);

    //if same dep than current node depth continue in the while
    if (dep == depth)
    {
      fr = 0;
      continue;
    }
    else
      break;
  }

  if (dep < depth)
    return parent_->build_node(depth - 1, dep, curr_pos);
  //else
  PTrie p(this, file_size_, chunk_, data_start_);
  std::shared_ptr<PTrie> pt = std::make_shared<PTrie>(p);
  std::get<2>(v2_[v2_.size()-1]) = pt;
  return pt->build_node(depth+1, dep, curr_pos);
}

/**
 * Moves currsor after the next "," to read with atoi
 */
void PTrie::next_comma(int& curr_pos)
{
  while (*(chunk_+curr_pos++) != ',')
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

/**
 * Would be better to return a pointer to the vector
 */
std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search0(const std::string& word)
{
  std::vector<std::tuple<std::string, unsigned long, unsigned int>> result;
  PTrie* pt = this;
  size_t v_size = pt->v_.size();
  if (!v_size)
    return result;
  size_t ws = word.size();
  int word_i = 0;
  int v_i = 0;
  while (1)
  {
    auto s = std::get<STRING>(pt->v_[v_i]);
    size_t ss = s.size();
    for (int j = 0; j < ss; j++)
    {
      if (s[j] == word[word_i])
      {
        word_i++;
        if (word_i == ws)
        {
          if (j == ss - 1 && std::get<FREQUENCE>(pt->v_[v_i]) != 0)
          {
            result.emplace_back(word, std::get<FREQUENCE>(pt->v_[v_i]), 0);
          }
          return result;
        }
        else if (j == ss - 1)
        {
          if (std::get<CHILD>(pt->v_[v_i]) == nullptr)
            return result;
          pt = std::get<CHILD>(pt->v_[v_i]).get();
          v_size = pt->v_.size();
          v_i = 0;
        }
      }
      else if (v_i == v_size - 1)
      {
        return result;
      }
      else
      {
        v_i++;
        break;
      }
    }
  }
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
