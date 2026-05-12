// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:flat.hierarchy.unordered;
import std;
import :decl;

export namespace iuic::tree::tag {
struct unordered : tag_qualifier__ {};
}; // namespace iuic::tree::tag

namespace iuic::tree::hierarchy {

struct unordered_base {
  using index_t = std::size_t; // TODO : replace to template papam
  struct node_t {
    static constexpr index_t npos = std::numeric_limits<index_t>::max();
    static constexpr index_t root = npos - 1;
    index_t parent{root};
    index_t left{npos};
    index_t right{npos};
    index_t ch_first{npos};
    index_t ch_last{npos};
  };

  std::vector<node_t> hierarchy__;
  index_t root_end{0};
};

export struct unordered;
}; // namespace iuic::tree::hierarchy

export namespace iuic::tree {

template <> struct base_iterator<hierarchy::unordered> {};
}; // namespace iuic::tree
