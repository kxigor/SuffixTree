#pragma once

#include <iostream>
#include <unordered_set>

#include "SuffixTree.hpp"

template <typename SuffixTreeType>
class SuffixTreeVerifier {
  using SuffixTreeT = SuffixTreeType;
  using StringT = typename SuffixTreeT::StringT;
  using SymT = typename SuffixTreeT::SymT;
  using NodeT = typename SuffixTreeT::Node;

 public:
  explicit SuffixTreeVerifier(const SuffixTreeT& tree) : tree_(tree) {}

  bool Verify() {
    std::unordered_set<StringT> suffixes;
    GenerateSuffixes(suffixes);

    for (const auto& suffix : suffixes) {
      if (!IsSuffixInTree(suffix)) {
        return false;
      }
    }
    return true;
  }

 private:
  void GenerateSuffixes(std::unordered_set<StringT>& suffixes) {
    for (std::size_t i = 0; i < tree_.input_str_.size(); ++i) {
      suffixes.insert(tree_.input_str_.substr(i));
    }
  }

  [[nodiscard]] bool IsSuffixInTree(const StringT& suffix) const {
    const NodeT* current = tree_.root_;
    std::size_t pos = 0;
    while (pos < suffix.size()) {
      const SymT kSym = suffix[pos];
      if (!current->IsChildExist(kSym)) {
        return false;
      }
      current = current->childs.at(kSym);
      const std::size_t kEdgeLen = std::min(current->len, suffix.size() - pos);
      for (std::size_t i = 0; i < kEdgeLen; ++i) {
        if (tree_.input_str_[current->start + i] != suffix[pos + i]) {
          return false;
        }
      }
      pos += kEdgeLen;
    }
    return pos == suffix.size();
  }

  const SuffixTreeT& tree_;
};