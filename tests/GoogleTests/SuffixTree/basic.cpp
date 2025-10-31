#include <gtest/gtest.h>

#include <sstream>

#include "include/SuffixTree.hpp"
#include "include/SuffixTreePrinter.hpp"
#include "include/SuffixTreeVeriefer.hpp"

std::string TestTree(const std::string& s, const std::string& t);

std::string TestTree(const std::string& s, const std::string& t) {
  suffix_tree::SuffixTree tree(s + t);
  suffix_tree::SuffixTreeVerifier verifier(tree);
  if (not verifier.Verify()) {
    return "";
  }
  suffix_tree::SuffixTreePrinter printer(tree, s, t);
  std::stringstream outbuffer;
  printer.Print(outbuffer);
  return outbuffer.str();
}

TEST(Suftree, Basic1) {
  ASSERT_EQ(TestTree("ab$", "ac#"),
            R"(8
0 1 2 3
0 0 2 3
0 0 0 1
3 0 1 3
3 1 1 3
0 0 1 3
0 1 1 3
)");
}

TEST(Suftree, Basic2) {
  ASSERT_EQ(TestTree("aba$", "baab#"),
            R"(14
0 1 4 5
0 0 3 4
0 0 0 1
3 0 3 4
3 1 2 5
3 0 1 2
6 1 4 5
6 0 2 4
0 0 1 2
9 1 4 5
9 0 2 3
11 0 3 4
11 1 2 5
)");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}