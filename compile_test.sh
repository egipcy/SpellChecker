#! /bin/sh

g++ -std=c++17 tests/test1.cc src/ptrie.cc src/bloom.cc -o test1
g++ -std=c++17 tests/test2.cc src/ptrie.cc src/bloom.cc -o test2
g++ -std=c++17 tests/test3.cc src/ptrie.cc src/bloom.cc -o test3
g++ -std=c++17 tests/test4.cc src/ptrie.cc src/bloom.cc -o test4
g++ -std=c++17 tests/test5.cc src/ptrie.cc src/bloom.cc -o test5
g++ -std=c++17 tests/test6.cc src/ptrie.cc src/bloom.cc -o test6
g++ -std=c++17 tests/test7.cc src/bloom.cc -o test7
