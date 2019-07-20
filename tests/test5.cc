#include <iostream>
#include <cassert>
#include <climits>

#include "../src/ptrie.hh"

int main()
{
  std::cout << "\033[1;37;40m" << "Damereau-Levenshtein" << "\033[0m" << std::endl;
  
  /*unsigned int l;
  unsigned int l2;

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnour", 15, 1);
  assert(l == 1 && l2 == 1);

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 15, 1);
  assert(l == 1 && l2 == 2);

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 0, 1);
  assert(l == 1 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 2, 1);
  assert(l == 1 && l2 == 2);

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 1, 1);
  assert(l == 1 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 15, 0);
  assert(l == 1 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("bonjour", "bojnour", "bojnours", 15, 1);
  assert(l == 1 && l2 == 2);

  std::tie(l, l2) = damereau_levenshtein("coucou", "loulou", "loulou", 15, 0);
  assert(l == 2 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("coucou", "relouluo", "relouluo", 15, 1);
  assert(l == 5 && l2 == 5);

  std::tie(l, l2) = damereau_levenshtein("coucou", "relouluo", "relouluo", 5, 1);
  assert(l == 5 && l2 == 5);

  std::tie(l, l2) = damereau_levenshtein("coucou", "reluoluo", "reluoluo", 3, 1);
  assert(l == 4 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("coucou", "coucou", "coucou", 0, 1);
  assert(l == 0 && l2 == 0);

  std::tie(l, l2) = damereau_levenshtein("coucou", "couco", "coucou", 1, 1);
  assert(l == 1 && l2 == 0);

  std::tie(l, l2) = damereau_levenshtein("coucou", "couco", "coucou", 1, 0);
  assert(l == 1 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("coucou", "coucou", "coucoulol", 2, 1);
  assert(l == 0 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("coucou", "coucou", "coucoulol", 3, 1);
  assert(l == 0 && l2 == 3);

  std::tie(l, l2) = damereau_levenshtein("c", "c", "czefzeg", 0, 1);
  assert(l == 0 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("c", "b", "bzefzeg", 1, 1);
  assert(l == 1 && l2 == UINT_MAX);

  std::tie(l, l2) = damereau_levenshtein("c", "b", "b", 0, 1);
  assert(l == 1 && l2 == UINT_MAX);
  */
  std::cout << "OK" << std::endl;
  
  return 0;
}