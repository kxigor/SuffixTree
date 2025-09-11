#pragma once

#include <cassert>
#include <concepts>
#include <cstdlib>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

template <typename StringType = std::string>
requires requires(StringType& str, const StringType& cstr, std::size_t pos) {
  typename StringType::value_type;
  typename StringType::size_type;
  { str[pos] } -> std::same_as<typename StringType::value_type&>;
  { cstr[pos] } -> std::same_as<const typename StringType::value_type&>;
  { str.size() } -> std::same_as<typename StringType::size_type>;
  requires std::is_move_constructible_v<StringType>;
}
class SuffixTree {
  /*============================= Friends =============================*/
  template <typename U>
  friend class SuffixTreeVisualizer;
  template <typename U>
  friend class SuffixTreeVerifier;
  template <typename U>
  friend class SuffixTreePrinter;

  friend class TrackedNode;

  /*======================== Usings/Constants =========================*/
  using StringT = StringType;
  using SymT = StringT::value_type;
  using SizeT = StringT::size_type;

  struct Node;

  struct TrackedNode {
    /*========= Construct =========*/
    TrackedNode(Node* base, Node* curr, SizeT offset, SuffixTree* tree)
        : base(base), curr(curr), offset(offset), tree(tree) {}

    /*========= Modifiers =========*/
    void FollowLink() {
      assert(IsVertex());
      base = base->link;
      curr = nullptr;
      offset = 0;
    }

    void FollowNext(SymT sym) {
      if (offset == 0) {
        curr = base->childs[sym];
      }
      ++offset;
      UpdateTrackIfItNecessary();
      assert(!curr || offset < curr->len);
    }

    void UpdateTrackIfItNecessary() {
      if (offset >= curr->len) {
        UpdateTrack();
      }
    }

    [[nodiscard]] Node* InsertNodeInOffset() const {
      Node* separate_node = Node::GetNew(base, curr->start, offset);
      tree->UpdateNodeToAttach(separate_node);

      const SymT kBeginSym = tree->input_str_[curr->start];
      const SymT kSeparateSym = tree->input_str_[curr->start + offset];

      curr->start += offset;
      if (!curr->IsLeaf()) {
        curr->len -= offset;
      }

      base->childs[kBeginSym] = separate_node;
      separate_node->childs[kSeparateSym] = curr;
      curr->parent = separate_node;

      return separate_node;
    }

    void CalculateTrack(const Node* node) {
      assert(!node->IsLeaf());

      SizeT start = node->start;
      SizeT len = node->len;

      if (node->parent == tree->root_) {
        assert(len != 0);
        ++start;
        --len;
      }

      base = node->parent->link;
      curr = nullptr;
      offset = 0;

      if (len > 0) {
        curr = base->childs[tree->input_str_[start]];
      }
      while (curr != nullptr && len >= curr->len) {
        start += curr->len;
        len -= curr->len;
        offset = curr->len;
        UpdateTrackIfItNecessary();
        if (len > 0) {
          curr = base->childs[tree->input_str_[start]];
        }
      }

      if (curr != nullptr) {
        offset = len;
      }
    }

    /*========== Checks ===========*/
    [[nodiscard]] bool IsVertex() const noexcept { return offset == 0; }

    [[nodiscard]] bool IsNextSame(SymT sym) const {
      if (offset == 0) {
        return base->IsChildExist(sym);
      }
      assert(curr != nullptr);
      return tree->input_str_[curr->start + offset] == sym;
    }

    [[nodiscard]] bool IsBaseRoot() const noexcept {
      return base == tree->root_;
    }

   private:
    void UpdateTrack() {
      const SymT kBaseSym = tree->input_str_[curr->start];
      const SymT kCurrSym = tree->input_str_[curr->start + offset];
      offset -= curr->len;
      assert(base->IsChildExist(kBaseSym));
      base = curr;
      curr = nullptr;
      if (offset > 0) {
        assert(base->IsChildExist(kCurrSym));
        curr = base->childs[kCurrSym];
      }
    }

    /*========== Fields ===========*/
   public:
    Node* base{};
    Node* curr{};
    SizeT offset{};
    SuffixTree* tree{};
  };

  struct Node {
    /*===== Usings/Constants ======*/
    using MapT = std::unordered_map<SymT, Node*>;
    static constexpr SizeT kLeaf = std::numeric_limits<SizeT>::max();

    /*========= Construct =========*/
    [[nodiscard]] static Node* GetNew(Node* parent, SizeT start, SizeT len) {
      Node* node = new Node{};

      node->parent = parent;
      node->start = start;
      node->len = len;
      node->link = nullptr;

      return node;
    }

    [[nodiscard]] static Node* GetNewLeaf(Node* parent, SizeT start) {
      return GetNew(parent, start, kLeaf);
    }

    /*========== LookUp ===========*/
    [[nodiscard]] bool IsChildExist(SymT sym) const {
      return childs.contains(sym);
    }

    [[nodiscard]] bool IsLeaf() const noexcept { return len == kLeaf; }

    /*========== Fields ===========*/
    SizeT start{};
    SizeT len{};

    Node* link{};
    Node* parent{};
    MapT childs;
  };

  /*========================== Constructors ===========================*/
 public:
  SuffixTree() = delete;
  explicit SuffixTree(StringT input_str)
      : root_(Node::GetNew(nullptr, 0, 0)),
        input_str_(std::move(input_str)),
        tracked_node_(root_, nullptr, 0, this),
        str_pos_(0) {
    root_->link = root_;
    for (str_pos_ = 0; str_pos_ < input_str_.size(); ++str_pos_) {
      ProcessSymbol();
    }
  }
  ~SuffixTree() {
    std::vector<Node*> stack;
    stack.push_back(root_);
    while (!stack.empty()) {
      Node* curr = stack.back();
      stack.pop_back();
      for (auto& [sym, child] : curr->childs) {
        stack.push_back(child);
      }
      delete curr;
    }
  }

 private:
  /*============================== Impl ===============================*/

  void ProcessSymbol() {
    const SymT kSym = input_str_[str_pos_];
    is_need_to_continue_processing_ = true;
    while (is_need_to_continue_processing_) {
      if (tracked_node_.IsNextSame(kSym)) {
        tracked_node_.FollowNext(kSym);
        is_need_to_continue_processing_ = false;
      } else {
        if (tracked_node_.IsVertex()) {
          HandleVertexCase(kSym);
        } else {
          HandleEdgeCase(kSym);
        }
      }
    }
  }

  void HandleVertexCase(SymT sym) {
    Node* parent = tracked_node_.base;
    Node* new_leaf = Node::GetNewLeaf(parent, str_pos_);
    parent->childs[sym] = new_leaf;
    is_need_to_continue_processing_ = !tracked_node_.IsBaseRoot();
    tracked_node_.FollowLink();
  }

  void HandleEdgeCase(SymT sym) {
    Node* separate_node = tracked_node_.InsertNodeInOffset();
    UpdateNodeToAttach(separate_node);
    tracked_node_.CalculateTrack(separate_node);

    Node* new_node = Node::GetNewLeaf(separate_node, str_pos_);
    separate_node->childs[sym] = new_node;

    if (tracked_node_.IsVertex()) {
      separate_node->link = tracked_node_.base;
    } else {
      SetNodeToAttach(separate_node);
    }
  }

  void SetNodeToAttach(Node* node_to_attach) {
    node_to_attach_ = node_to_attach;
  }

  void UpdateNodeToAttach(Node* link_for_node_to_attach) {
    if (node_to_attach_ != nullptr) {
      node_to_attach_->link = link_for_node_to_attach;
      node_to_attach_ = nullptr;
    }
  }

  /*============================= Fields ==============================*/
  Node* root_;
  StringT input_str_;
  TrackedNode tracked_node_;
  SizeT str_pos_;
  bool is_need_to_continue_processing_{};
  Node* node_to_attach_{};
};