#include <iostream>
#include <algorithm>
#include <memory>

#include "ptrie.hh"

PTrie::PTrie()
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
  if (length == 0)
    return search0(word);

  // TODO

  return std::vector<std::tuple<std::string, unsigned long, unsigned int>>();
}

void PTrie::serialize(std::ofstream& file)
{
  for (size_t i = 0; i < v_.size(); i++)
  {
    auto e = v_[i];

    file << std::get<STRING>(e) << std::endl;
    file << std::get<FREQUENCE>(e) << std::endl;
    if (std::get<CHILD>(e))
    {
      file << "0" << std::endl;
      std::get<CHILD>(e)->serialize(file);
    }
    else
      file << std::endl;

    if (i < v_.size() - 1)
      file << "1" << std::endl;
  }
  file << std::endl;
}

void PTrie::deserialize(std::ifstream& file)
{
  std::string line;

  if (!std::getline(file, line))
    return;
  std::string s = line;

  std::getline(file, line);
  unsigned long i = std::stoul(line);

  std::getline(file, line);
  std::shared_ptr<PTrie> ptrie = nullptr;
  if (line == "0")
  {
    ptrie = std::make_shared<PTrie>();
    ptrie->deserialize(file);
  }

  v_.push_back(std::make_tuple(s, ptrie, i));

  std::getline(file, line);
  if (line == "1")
    deserialize(file);
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
