// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree.orderable:orders;
import std;
import iuic.underlying.iterator;
import iuic.underlying.utils;
import :impl;

namespace iuic::tree {

export namespace order {
struct levelorder_t {
  static constexpr type type;
} levelorder;

struct preorder_t {
  static constexpr type type;
} preorder;

struct postorder_t {
  static constexpr type type;
} postorder;

}; // namespace order

template <> struct organizer<iuic::tree::order::unordered_t> {
  template <typename index_t>
  static void
  process(iuic::tree::hierarchy::basic_orderable<index_t> &basic_tree) {};
};

template <> struct organizer<iuic::tree::order::levelorder_t> {
  template <typename index_t>
  static void
  process(iuic::tree::hierarchy::basic_orderable<index_t> &basic_tree) {
    if (basic_tree.hierarchy.empty()) {
      return;
    }

    iuic::tree::hierarchy::basic_orderable<index_t> res;
    res.hierarchy.reserve(basic_tree.hierarchy.size());

    std::vector<index_t> current;
    std::vector<index_t> next;
    current.reserve(basic_tree.hierarchy.size() / 4);
    next.reserve(basic_tree.hierarchy.size() / 4);

    index_t end = basic_tree.hierarchy.size();
    index_t separator = end;
    index_t index = basic_tree.root_.ch_first;
    index_t parent = iuic::tree::hierarchy::indexed<index_t>::root;

    index_t ch_first = 0;
    index_t sibling = 0;
    index_t self = 0;

    for (; index < end;) {
      current.push_back(index);
      index = basic_tree.hierarchy[index].right;
    }
    current.push_back(separator);

    for (;;) {
      for (auto i : current) {
        if (i == separator) {

          if (parent > end) [[unlikely]] {
            res.root_.ch_first = 0;
            ch_first = res.hierarchy.size();
            res.root_.ch_last = ch_first - 1;

            sibling = 0;
            self = 1;

            res.hierarchy[0].parent = parent;
            for (; self < ch_first; ++self, ++sibling) {
              res.hierarchy[sibling].right = self;
              res.hierarchy[self].left = sibling;
              res.hierarchy[self].parent = parent;
            };
            parent = 0;
          } else {
            if (ch_first != res.hierarchy.size()) {
              auto &node = res.hierarchy[parent];
              node.ch_first = ch_first;
              sibling = ch_first;
              ch_first = res.hierarchy.size();
              node.ch_last = ch_first - 1;

              self = sibling + 1;
              res.hierarchy[sibling].parent = parent;

              for (; self < ch_first; ++self, ++sibling) {
                res.hierarchy[sibling].right = self;
                res.hierarchy[self].left = sibling;
                res.hierarchy[self].parent = parent;
              };
            }
            ++parent;
          }
          continue;
        }

        index = basic_tree.hierarchy[i].ch_first;

        for (; index < end;) {
          next.push_back(index);
          index = basic_tree.hierarchy[index].right;
        }

        res.hierarchy.push_back({.data = basic_tree.hierarchy[i].data});

        next.push_back(separator);
      };

      std::swap(current, next);
      next.clear();

      if (current.empty()) {
        std::swap(res, basic_tree);
        return;
      }
    };
  };
};

template <> struct organizer<iuic::tree::order::postorder_t> {
  template <typename index_t>
  static void
  process(iuic::tree::hierarchy::basic_orderable<index_t> &basic_tree) {

    // do job
    iuic::tree::hierarchy::basic_orderable<index_t> res;
    res.hierarchy.reserve(basic_tree.hierarchy.size());

    index_t npos = iuic::tree::hierarchy::indexed<index_t>::npos;
    index_t selected = basic_tree.root_.ch_first;

    struct siblings_t {
      index_t first, last;
    };

    std::vector<index_t> stack;
    std::vector<siblings_t> siblings;

    // siblings.push_back({}); // FOR ROOT

    for (; selected < npos;) {
      stack.push_back(selected);
      siblings.push_back({npos, npos});

      selected = basic_tree.hierarchy[selected].ch_first;
    };

    bool up{false};

    for (;;) {
      auto &node = basic_tree.hierarchy[stack.back()];
      stack.pop_back();
      auto &sib = siblings.back();
      index_t insert_index = res.hierarchy.size();

      if (up) {
        auto &self_siblings = siblings.at(siblings.size() - 2);

        res.hierarchy.push_back({.left = self_siblings.last,
                                 .ch_first = sib.first,
                                 .ch_last = sib.last,
                                 .data = node.data});

        if (sib.first != npos) {
          auto range = sib;
          ++range.last;
          for (; range.first < npos;
               range.first = res.hierarchy[range.first].right) {
            res.hierarchy[range.first].parent = insert_index;
          }
        }

        if (self_siblings.first == npos) {
          self_siblings.first = insert_index;
        }

        if (self_siblings.last != npos) {
          res.hierarchy[self_siblings.last].right = insert_index;
        }

        // finalize
        self_siblings.last = insert_index;
        siblings.pop_back();

      } else {
        auto &sib = siblings.back();

        res.hierarchy.push_back({.left = sib.last, .data = node.data});

        if (sib.first == npos) {
          sib.first = insert_index;
        }

        if (sib.last != npos) {
          res.hierarchy[sib.last].right = insert_index;
        }

        sib.last = insert_index;
      }

      if (node.right < npos) {
        up = false;
        stack.push_back(node.right);
        selected = basic_tree.hierarchy[node.right].ch_first;

        for (; selected < npos;) {
          stack.push_back(selected);
          siblings.push_back({npos, npos});

          selected = basic_tree.hierarchy[selected].ch_first;
        };

      } else if (node.parent < npos) {
        up = true;

      } else {
        auto &sib = siblings.back();

        res.root_.ch_first = sib.first;
        res.root_.ch_last = sib.last;
        std::swap(res, basic_tree);
        return;
      }
    }
  }
};

template <> struct organizer<iuic::tree::order::preorder_t> {
  template <typename index_t>
  static void
  process(iuic::tree::hierarchy::basic_orderable<index_t> &basic_tree) {
    //

    if (basic_tree.hierarchy.empty()) {
      return;
    }

    iuic::tree::hierarchy::basic_orderable<index_t> res;
    res.hierarchy.reserve(basic_tree.hierarchy.size());

    std::vector<index_t> stack; // parent
    index_t selected = basic_tree.root_.ch_first;
    index_t npos = iuic::tree::hierarchy::indexed<index_t>::npos;

    for (;;) {
      auto &node = basic_tree.hierarchy[selected];
      index_t insert_index = res.hierarchy.size();

      if (stack.empty()) {
        if (res.root_.ch_last < npos) {
          res.hierarchy.push_back(
              {.left = res.root_.ch_last, .data = node.data});

          res.hierarchy[res.root_.ch_last].right = insert_index;
          res.root_.ch_last = insert_index;

        } else {
          res.hierarchy.push_back({.data = node.data});
          res.root_.ch_first = insert_index;
          res.root_.ch_last = insert_index;
        }
      } else {
        auto pindex = stack.back();
        auto &pnode = res.hierarchy[pindex];

        if (pnode.ch_last < npos) {
          res.hierarchy.push_back(
              {.parent = pindex, .left = pnode.ch_last, .data = node.data});

          res.hierarchy[pnode.ch_last].right = insert_index;

          pnode.ch_last = insert_index;

        } else {
          res.hierarchy.push_back({.parent = pindex, .data = node.data});
          pnode.ch_first = insert_index;
          pnode.ch_last = insert_index;
        }
      }

      if (node.ch_first < npos) {
        stack.push_back(insert_index);
        selected = node.ch_first;
      } else if (node.right < npos) {
        selected = node.right;
      } else {
        for (;;) {
          auto &snode = basic_tree.hierarchy[selected];

          if (snode.parent < npos) {
            auto &snode_inner = basic_tree.hierarchy[snode.parent];

            iuic::utils::assert([&] { return not stack.empty(); },
                                "The tree invariant is violated.");

            stack.pop_back();
            if (snode_inner.right < npos) {
              selected = snode_inner.right;
              break;
            } else {
              selected = snode.parent;
            }
          } else {
            std::swap(res, basic_tree);
            return;
          }
        }
      }
    };
  };
};

}; // namespace iuic::tree
