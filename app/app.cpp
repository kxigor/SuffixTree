#include <iostream>
#include <string>

#include "include/SuffixTree.hpp"
#include "include/SuffixTreePrinter.hpp"

int main() {
  std::string inp_1, inp_2;
  std::cin >> inp_1 >> inp_2;
  const suffix_tree::SuffixTree kTree(inp_1 + inp_2);
  suffix_tree::SuffixTreePrinter printer(kTree, inp_1, inp_2);
  printer.Print(std::cout);
}