#include <iostream>
#include <string>

#include "SuffixTree.hpp"
#include "SuffixTreePrinter.hpp"

int main() {
  std::string inp_1;
  std::string inp_2;
  std::cin >> inp_1 >> inp_2;
  const SuffixTree kTree(inp_1 + inp_2);
  SuffixTreePrinter printer(kTree, inp_1, inp_2);
  printer.Print(std::cout);
}