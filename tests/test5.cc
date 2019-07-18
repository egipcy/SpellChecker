#include <iostream>
#include <assert.h>

#include "../src/ptrie.hh"

int main()
{
  std::cout << "\033[1;37;40m" << "Damereau-Levenshtein" << "\033[0m" << std::endl;
  
  unsigned int l;
  bool b;

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnour", 15, 1);
  assert(l == 1 && b);

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 15, 1);
  assert(l == 1 && b);

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 0, 1);
  assert(l == 1 && !b);

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 2, 1);
  assert(l == 1 && b);

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 1, 1);
  assert(l == 1 && !b);

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 15, 0);
  assert(l == 1 && !b);

  std::tie(l, b) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 15, 1);
  assert(l == 1 && b);

  std::tie(l, b) = damereau_levenshtein("coucou", "loulou", "loulou", 15, 0);
  assert(l == 2 && !b);

  std::tie(l, b) = damereau_levenshtein("coucou", "relouluo", "relouluo", 15, 1);
  assert(l == 5 && b);

  std::tie(l, b) = damereau_levenshtein("coucou", "relouluo", "relouluo", 5, 1);
  assert(l == 5 && b);

  std::tie(l, b) = damereau_levenshtein("coucou", "reluoluo", "reluoluo", 3, 1);
  assert(l == 4 && !b);

  std::tie(l, b) = damereau_levenshtein("coucou", "coucou", "coucou", 0, 1);
  assert(l == 0 && b);

  std::tie(l, b) = damereau_levenshtein("coucou", "couco", "coucou", 1, 1);
  assert(l == 1 && b);

  std::tie(l, b) = damereau_levenshtein("coucou", "couco", "coucou", 1, 0);
  assert(l == 1 && !b);

  std::tie(l, b) = damereau_levenshtein("coucou", "coucou", "coucoulol", 2, 1);
  assert(l == 0 && !b);

  std::tie(l, b) = damereau_levenshtein("coucou", "coucou", "coucoulol", 3, 1);
  assert(l == 0 && b);

  std::tie(l, b) = damereau_levenshtein("c", "c", "czefzeg", 0, 1);
  assert(l == 0 && !b);

  std::tie(l, b) = damereau_levenshtein("c", "b", "bzefzeg", 1, 1);
  assert(l == 1 && !b);

  std::tie(l, b) = damereau_levenshtein("c", "b", "b", 0, 1);
  assert(l == 1 && !b);

  std::cout << "OK" << std::endl;
  
  return 0;
}