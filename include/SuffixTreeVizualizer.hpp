#pragma once

#include <fstream>

#include "SuffixTree.hpp"

namespace suffix_tree {
template <typename SuffixTreeType>
class SuffixTreeVisualizer {
  using SuffixTreeT = SuffixTreeType;
  using StringT = typename SuffixTreeT::StringT;
  using SymT = typename SuffixTreeT::SymT;
  using NodeT = typename SuffixTreeT::Node;

 public:
  explicit SuffixTreeVisualizer(const SuffixTreeT& tree) : tree_(tree) {}

  void Visualize(const StringT& dot_filename = "suftree.dot",
                 const StringT& png_filename = "suftree.png") {
    PrintDot(dot_filename);

    const StringT kCommand =
        "dot -Tpng " + dot_filename + " -o " + png_filename;
    system(kCommand.c_str());
  }

 private:
  void GenerateDot(NodeT* node, std::ostream& out) const {
    if (node == nullptr) {
      return;
    }

    out << "  node" << node << " [label=\"";
    if (node == tree_.root_) {
      out << "root";
    } else {
      out << tree_.input_str_.substr(node->start, node->len);
    }
    out << "\"];\n";

    for (const auto& [sym, child] : node->childs) {
      GenerateDot(child, out);
      out << "  node" << node << " -> node" << child << " [label=\"" << sym
          << "\"];\n";
    }

    if (node->link != nullptr) {
      out << "  node" << node << " -> node" << node->link
          << " [style=dotted, color=red];\n";
    }
  }

  void PrintDot(const StringT& filename) const {
    std::ofstream out(filename);
    out << "digraph SuffixTree {\n";
    GenerateDot(tree_.root_, out);
    out << "}\n";
  }

  const SuffixTreeT& tree_;
};
}  // namespace suffix_tree