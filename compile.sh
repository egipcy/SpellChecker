#! /bin/sh

g++ src/text-mining-compiler.cc src/ptrie.cc -o TextMiningCompiler
g++ src/text-mining-app.cc src/ptrie.cc -o TextMiningApp
