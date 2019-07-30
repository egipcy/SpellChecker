#! /bin/sh

g++ -std=c++17 -O3 src/text-mining-compiler.cc src/ptrie.cc src/bloom.cc -o TextMiningCompiler
g++ -std=c++17 -O3 -pg src/text-mining-app.cc src/ptrie.cc src/bloom.cc -o TextMiningApp
