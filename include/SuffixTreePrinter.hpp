#pragma once

#include <algorithm>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "SuffixTree.hpp"

namespace suffix_tree {
template <typename SuffixTreeType>
class SuffixTreePrinter {
  using SuffixTreeT = SuffixTreeType;
  using StringT = typename SuffixTreeT::StringT;
  using SymT = typename SuffixTreeT::SymT;
  using NodeT = typename SuffixTreeT::Node;

 public:
  SuffixTreePrinter(const SuffixTreeT& tree, const StringT& input_1,
                    const StringT& input_2)
      : tree_(tree), input_1_(input_1), input_2_(input_2) {}

  void Print(std::ostream& out) {
    OrderedDfs(tree_.root_);
    out << ordered_nodes_.size() << "\n";
    for (std::size_t i = 1; i < ordered_nodes_.size(); ++i) {
      NodeT* node = ordered_nodes_[i];
      NodeT* parent = node->parent;
      const std::size_t kPapentId = node_id_[parent];
      const std::size_t kSide = (node->start < input_1_.size()) ? 0 : 1;
      std::size_t left_border = node->start;
      std::size_t right_border{};
      if (node->IsLeaf()) {
        right_border =
            (kSide == 0 ? input_1_.size() : input_1_.size() + input_2_.size());
      } else {
        right_border = node->start + node->len;
      }
      if (kSide == 1) {
        left_border -= input_1_.size();
        right_border -= input_1_.size();
      }
      out << kPapentId << " " << kSide << " " << left_border << " "
          << right_border << "\n";
    }
  }

 private:
  void OrderedDfs(NodeT* node) {
    if (node == nullptr) {
      return;
    }
    node_id_[node] = ordered_nodes_.size();
    ordered_nodes_.push_back(node);
    std::vector<std::pair<SymT, NodeT*>> sorted_edges(node->childs.begin(),
                                                      node->childs.end());
    std::ranges::sort(sorted_edges);
    for (const auto& [sym, child] : sorted_edges) {
      OrderedDfs(child);
    }
  }

  const SuffixTreeT& tree_;
  const StringT& input_1_;
  const StringT& input_2_;
  std::vector<NodeT*> ordered_nodes_;
  std::unordered_map<NodeT*, size_t> node_id_;
};
}  // namespace suffix_tree