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
#include <climits>
#include <cassert>

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

  // Add chars in vocabulary
  for (auto c: word)
    if (vocabulary_.find(c) == std::string::npos)
      vocabulary_ += c;

#if BLOOM_FILTER
  // Add word to bloom filter
  filter_.add(word);
#endif

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
  std::string pos = std::to_string(file.tellp()); // Offset to the data
  save_edges(file);

#if BLOOM_FILTER
  file.write("\n", 1); // Delimiter for Bloom filter
  std::string bloomfilter = filter_.to_string();
  file.write(bloomfilter.c_str(), bloomfilter.size());
#endif

  file.write("\n", 1); // Delimiter for vocabulary
  file.write(vocabulary_.c_str(), vocabulary_.size());

  file.write("\n", 1); // Delimiter between data and offset
  file.write(pos.c_str(), pos.size()); // Append the data offset at the end
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
  char* chunk = reinterpret_cast<char*>(mmap(NULL, file_size, PROT_READ, MAP_FILE | MAP_PRIVATE | MAP_POPULATE, fd, 0));
  ::close(fd);

  // Read the numbers at the end of the file
  auto last_newline = std::string(chunk).find_last_of("\n");
  int data_start = atoi(chunk + last_newline); // the datas starts at *(chunk + data_start)

  // Read vocabulary
  auto penultimate_newline = std::string(chunk).find_last_of("\n", last_newline - 1);
  vocabulary_ = std::string(chunk + penultimate_newline + 1, last_newline - penultimate_newline - 1);

#if BLOOM_FILTER
  // Read Bloom filter
  auto antepenultimate_newline = std::string(chunk).find_last_of("\n", penultimate_newline - 1);
  filter_.from_string(std::string(chunk + antepenultimate_newline + 1, penultimate_newline - antepenultimate_newline - 1));
#endif

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
        if (*(chunk+curr_pos) == ',')
          fr = strtoul(chunk+(++curr_pos), nullptr, 10);
      pt->v_.emplace_back(std::string(data_chunk + of, co), nullptr, fr);
      if (curr_pos >= data_start-1)
        return;
      
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
PTrie::search(const std::string& word, unsigned int length)
{
  std::vector<std::tuple<std::string, unsigned long, unsigned int>> ret;

  if (length == 0)
    ret = search0(word, "", length);

  else if (length == 1)
    ret = search1(word, length);

  else
  {
    std::vector<std::vector<unsigned int>> d(1);
    d[0] = std::vector<unsigned int>(word.size() + 1);

    for (size_t j = 1; j <= word.size(); j++)
      d[0][j] = j;
    
    ret = searchN(d, word, "", length);
  }

  // Sort results
  std::sort(ret.begin(), ret.end(),
    [](const std::tuple<std::string, unsigned long, unsigned int>& a,
      const std::tuple<std::string, unsigned long, unsigned int>& b)
    {
      if (std::get<2>(a) != std::get<2>(b))
        return std::get<2>(a) < std::get<2>(b);
      if (std::get<1>(a) != std::get<1>(b))
        return std::get<1>(a) > std::get<1>(b);
      return std::get<0>(a) < std::get<0>(b);
    }
  );

  return ret;
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search0(const std::string& word, const std::string& prefix_w, unsigned int origin_length)
{
#if BLOOM_FILTER
  if (filter_.has_word(word))
    return search0_rec(word, prefix_w, origin_length);

  return {};
#else
  return search0_rec(word, prefix_w, origin_length);
#endif
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search0_rec(const std::string& word, const std::string& prefix_w, unsigned int origin_length)
{
  auto pos = dicho(v_, word);

  if (pos != std::numeric_limits<size_t>::max())
  {
    const std::string& w = std::get<STRING>(v_[pos]);
    auto freq = std::get<FREQUENCE>(v_[pos]);

    if (w.size() == word.size() && freq != 0)
      return {std::tuple<std::string, unsigned long, unsigned int>(prefix_w + w, freq, origin_length)};

    if (w.size() < word.size())
    {
      auto child = std::get<CHILD>(v_[pos]);
      if (child)
        return child->search0_rec(word.substr(w.size()), prefix_w + w, origin_length);
    }
  }

  return {};
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search1(const std::string& word, unsigned int origin_length)
{
  auto ret = search0(word, "", 0);

  auto new_word = word;

  // Replacement
  for (size_t i = 0; i < word.size(); i++)
  {
    for (auto new_c: vocabulary_)
      if (word[i] != new_c)
      {
        new_word[i] = new_c;
        extend(ret, search0(new_word, "", origin_length));
      }
    new_word[i] = word[i];
  }

  // Insertion
  for (size_t i = 0; i < word.size() + 1; i++)
  {
    new_word.insert(i, 1, 0);
    for (auto new_c: vocabulary_)
    {
      new_word[i] = new_c;
      extend(ret, search0(new_word, "", origin_length));
    }
    new_word = word;
  }

  // Deletion
  for (size_t i = 0; i < word.size(); i++)
  {
    new_word = word;
    new_word.erase(i, 1);
    extend(ret, search0(new_word, "", origin_length));
  }

  // Inversion
  for (size_t i = 1; i < word.size(); i++)
    if (word[i - 1] != word[i])
    {
      new_word = word;
      char c = new_word[i - 1];
      new_word[i - 1] = new_word[i];
      new_word[i] = c;
      extend(ret, search0(new_word, "", origin_length));
    }

  // Erase duplicates
  for (size_t i = 0; i < ret.size(); i++)
    for (size_t j = i + 1; j < ret.size(); j++)
      if (std::get<STRING>(ret[i]) == std::get<STRING>(ret[j]))
        ret.erase(ret.cbegin() + j--);
  return ret;
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::searchN(const std::vector<std::vector<unsigned int>>& d, const std::string& word, const std::string& prefix_w, unsigned int origin_length)
{
  std::vector<std::tuple<std::string, unsigned long, unsigned int>> ret;

  for (const auto& e: v_)
  {
    auto w = std::get<STRING>(e);

    auto dl = damerau_levenshtein(d, w, prefix_w.size() == 0 ? 0 : prefix_w[prefix_w.size() - 1], word, origin_length); // DL between tot_w and word

    bool b = false;
    for (auto e: dl[dl.size() - 1])
      if (e <= origin_length)
      {
        b = true;
        break;
      }
    if (b) // We can pursue
    {
      auto freq = std::get<FREQUENCE>(e);
      auto child = std::get<CHILD>(e);

      auto tot_w = prefix_w + w;

      auto l = dl[tot_w.size()][word.size()];

      if (freq != 0 && dl[tot_w.size()][word.size()] <= origin_length) // tot_w is accepted
        ret.emplace_back(tot_w, freq, l);

      if (child) // let's continue
      {
        auto v = child->searchN(dl, word, tot_w, origin_length);

        // ret = ret + v
        extend(ret, v);
      }
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

std::vector<std::vector<unsigned int>>
damerau_levenshtein(std::vector<std::vector<unsigned int>> dl, const std::string& w, const char lastchar_w, const std::string& word, unsigned int length)
{
  // 4 operations: insertion, deletion, edition, inversion
  // No need to exceed length in term of distance

  auto last_dsize = dl.size();

  // Add rows in table for each letter of w
  dl.reserve(dl.size() + w.size());
  for (size_t i = 0; i < w.size(); i++)
  {
    dl.emplace_back(word.size() + 1, UINT_MAX);
    dl[dl.size() - 1][0] = dl.size() - 1;
  }

  for (size_t i = last_dsize; i < dl.size(); i++)
  {
    unsigned int min_line = dl[i][0];
    for (size_t j = 1; j <= word.size(); j++)
    {
      unsigned sub_or_exact = w[i - last_dsize] == word[j - 1] ? 0 : 1;

      dl[i][j] = std::min({
        dl[i - 1][j] + 1,
        dl[i][j - 1] + 1,
        dl[i - 1][j - 1] + sub_or_exact});

      if (i > 1 && j > 1 && w[i - last_dsize] == word[j - 2] && (i == last_dsize ? lastchar_w : w[i - last_dsize - 1]) == word[j - 1])
        dl[i][j] = std::min(dl[i][j], dl[i - 2][j - 2] + sub_or_exact);
    }
  }

  /* Printing
  std::cout << "  ";
  for (size_t j = 1; j <= word.size(); j++)
    std::cout << word[j - 1] << " ";
  std::cout << std::endl;
  for (size_t i = 1; i < d.size(); i++)
  {
    std::string s = "/";
    if (w.size() >= d.size() - i)
      s = w[w.size() - d.size() + i];
    std::cout << s << " ";
    for (size_t j = 1; j <= word.size(); j++)
      std::cout << d[i][j] << " ";
    std::cout << std::endl;
  }
  std::cout << std::endl;
  */

  return dl;
}

size_t dicho(const std::vector<std::tuple<std::string, std::shared_ptr<PTrie>, unsigned long>>& v, const std::string& word)
{
  size_t start = 0;
  size_t end = v.size() - 1;

  while (start <= end)
  {
    size_t middle = (start + end) / 2;
    const std::string& w = std::get<PTrie::STRING>(v[middle]);

    int res = memcmp(word.c_str(), w.c_str(), std::min(word.size(), w.size()));


    if (res == 0)
      return middle;
    else if (res > 0)
      start = middle + 1;
    else
    {
      if (middle == 0)  // Handle overflow
        break;
      end = middle - 1;
    }
  }

  return std::numeric_limits<size_t>::max();
}

template <typename T>
void extend(std::vector<T>& ret, const std::vector<T>& v)
{
  if (v.size() != 0)
  {
    ret.reserve(ret.size() + v.size());
    ret.insert(ret.end(), v.begin(), v.end());
  }
}