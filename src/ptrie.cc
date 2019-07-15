#include <iostream>
#include <algorithm>

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
    v_.emplace_back(word, nullptr, true, frequence);
    return;
  }

  // else
  // Check word
  auto& tuple = v_[index];
  auto& w = std::get<0>(tuple);

  // word = prefix + suffix1
  // w = prefix + suffix2
  size_t i;
  for (i = 1; i < word.size() && i < w.size() && word[i] == w[i]; i++)
    continue;
  std::string prefix = word.substr(0, i);
  std::string suffix1 = word.substr(i, word.size() - i);
  std::string suffix2 = w.substr(i, w.size() - i);;

  // No suffixes
  if (suffix1.size() == 0 && suffix2.size() == 0)
  {
    std::get<2>(tuple) = true;
    std::get<3>(tuple) = frequence;
  }
  else
  {
    if (suffix2.size() != 0)
    {
      // Edge value set to shared prefix
      w = prefix;

      // Remember the children, the existence value and the frequence
      auto children = std::get<1>(tuple);
      auto value = std::get<2>(tuple);
      auto freq = std::get<3>(tuple);

      if (suffix1.size() != 0)
      {
        // w isn't a valid word anymore
        std::get<2>(tuple) = false;
        std::get<3>(tuple) = 0;
      }
      else  // Update frequence value
        std::get<3>(tuple) = frequence;

      // New node
      std::get<1>(tuple) = std::make_shared<PTrie>();
      std::get<1>(tuple)->v_.emplace_back(suffix2, children, value, freq);
    }
    // Insert suffix1 in children
    else if (!std::get<1>(tuple))
      std::get<1>(tuple) = std::make_shared<PTrie>();
    std::get<1>(tuple)->insert(suffix1, frequence);
  }
}

void PTrie::print(int nb_indent) const
{
  for (const auto& e: v_)
  {
    for (int i = 0; i < nb_indent; i++)
      std::cout << ' ';
    std::cout << std::get<0>(e) << " " << std::get<2>(e) << " f=" << std::get<3>(e) << std::endl;
    if (std::get<1>(e))
      std::get<1>(e)->print(nb_indent + 1);
  }
  if (nb_indent == 0)
    std::cout << std::endl;
}

void PTrie::sort()
{
  // Recursive calls
  for (auto e: v_)
    if (std::get<1>(e))
      std::get<1>(e)->sort();

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

    file << std::get<0>(e) << std::endl;
    file << std::get<2>(e) << std::endl;
    file << std::get<3>(e) << std::endl;
    if (std::get<1>(e))
    {
      file << "0" << std::endl;
      std::get<1>(e)->serialize(file);
    }
    else
      file << "1" << std::endl;

    if (i < v_.size() - 1)
      file << "no end" << std::endl;
  }
  file << "end" << std::endl;
}

void PTrie::deserialize(std::ifstream& file)
{
  std::string line;

  if (!std::getline(file, line))
    return;
  std::string s = line;

  std::getline(file, line);
  bool b = line != "0";

  std::getline(file, line);
  unsigned long i = std::stoul(line);

  std::getline(file, line);
  std::shared_ptr<PTrie> ptrie = nullptr;
  if (line == "0")
  {
    ptrie = std::make_shared<PTrie>();
    ptrie->deserialize(file);
  }

  v_.push_back(std::make_tuple(s, ptrie, b, i));

  std::getline(file, line);
  if (line == "no end")
    deserialize(file);
}

size_t PTrie::search_prefix(const std::string& word) const
{
  size_t i;
  for (i = 0; i < v_.size(); i++)
    if (std::get<0>(v_[i])[0] == word[0])
      break;
  return i;
}

std::vector<std::tuple<std::string, unsigned long, unsigned int>>
PTrie::search0(const std::string& word)
{
  // TODO

  return std::vector<std::tuple<std::string, unsigned long, unsigned int>>();
}

void print_result(const std::vector<std::tuple<std::string, unsigned long, unsigned int>>& result)
{
  // TODO
}