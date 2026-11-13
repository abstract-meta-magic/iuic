// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

import std;
import iuic.test;
import iuic.underlying.iterator;
import iuic.underlying.tree.orderable;

namespace {
struct test : iuic::test::unit<test> {
  using tree_t = iuic::tree::orderable<int>;
  static constexpr std::string_view name{"iuic::tree[orderable]"};
  static constexpr std::size_t execute_times{8};

  static tree_t generate_tree() {
    iuic::tree::orderable<int> res;
    auto seed = std::random_device{}();
    std::mt19937 device{seed};
    std::uniform_int_distribution<int> rng{0, 4};

    std::size_t insert_index{0};

    iuic::iterator::insert current{res.root()};
    current[insert_index]; // R-1 Always

    std::vector<decltype(current)> stack;

    for (; insert_index < 1000; ++insert_index) {
      int cmd = rng(device);
      switch (cmd) {
      case 0: {
        current[insert_index];
        break;
      }
      case 1: {
        if (not stack.empty()) {
          current = stack.back();
          stack.pop_back();
        }
      }
      case 2: {
        current &= insert_index;
        break;
      }
      case 3:
      case 4: {
        auto it = std::exchange(current, current(insert_index));
        stack.push_back(it);
        break;
      }
      };
    };

    return res;
  };

  static void body(iuic::test::utils utils) {

    tree_t tree = generate_tree();

    {
      tree_t levelorder_tree{tree};
      levelorder_tree.organize(iuic::tree::order::levelorder);
      utils.rq_true(is_isomorphic(tree, levelorder_tree),
                    "isomorphism for levelorder");
    }

    {
      tree_t postorder_tree{tree};
      postorder_tree.organize(iuic::tree::order::postorder);
      utils.rq_true(is_isomorphic(tree, postorder_tree),
                    "isomorphism for postorder");
    }

    {
      tree_t preorder_tree{tree};
      preorder_tree.organize(iuic::tree::order::preorder);
      utils.rq_true(is_isomorphic(tree, preorder_tree),
                    "isomorphism for preorder"); //
    }
  };

  static bool is_isomorphic(const tree_t &exp, const tree_t &dir) {
    if (exp.size() != dir.size()) {
      return false;
    }

    struct separator {};
    // TODO: body
    // use level order check
    std::vector<std::variant<iuic::iterator::basic<tree_t>, separator>>
        current_exp;
    std::vector<std::variant<iuic::iterator::basic<tree_t>, separator>>
        current_dir;
    std::vector<std::variant<iuic::iterator::basic<tree_t>, separator>>
        next_exp;
    std::vector<std::variant<iuic::iterator::basic<tree_t>, separator>>
        next_dir;

    for (auto it : iuic::iterator::range::from(iuic::iterator::basic{
             iuic::tree::childs_of(exp.root()), iuic::iterator::mode::sibling,
             iuic::iterator::extension::iterator<iuic::iterator::basic>{}})) {
      current_exp.push_back(it);
    }
    current_exp.push_back(separator{});

    for (auto it : iuic::iterator::range::from(iuic::iterator::basic{
             iuic::tree::childs_of(dir.root()), iuic::iterator::mode::sibling,
             iuic::iterator::extension::iterator<iuic::iterator::basic>{}})) {
      current_dir.push_back(it);
    }
    current_dir.push_back(separator{});

    if (current_exp.size() != current_dir.size()) {
      return false;
    }

    for (;;) {
      for (auto &&value : current_exp) {
        if (not std::holds_alternative<separator>(value)) {
          auto &it = std::get<iuic::iterator::basic<tree_t>>(value);

          for (auto it : iuic::iterator::range::from(iuic::iterator::basic{
                   iuic::tree::childs_of(it), iuic::iterator::mode::sibling,
                   iuic::iterator::extension::iterator<
                       iuic::iterator::basic>{}})) {
            next_exp.push_back(it);
          }
          next_exp.push_back(separator{});
        }
      }

      for (auto &&value : current_dir) {
        if (not std::holds_alternative<separator>(value)) {
          auto &it = std::get<iuic::iterator::basic<tree_t>>(value);

          for (auto it : iuic::iterator::range::from(iuic::iterator::basic{
                   iuic::tree::childs_of(it), iuic::iterator::mode::sibling,
                   iuic::iterator::extension::iterator<
                       iuic::iterator::basic>{}})) {
            next_dir.push_back(it);
          }
          next_dir.push_back(separator{});
        }
      }

      if (next_exp.size() != next_dir.size()) {
        return false;
      }

      if (next_dir.empty()) {
        return true;
      }

      for (std::size_t i{0}; i < next_exp.size(); ++i) {
        if (next_exp.at(i).index() != next_dir.at(i).index()) {
          return false;
        }
      };

      std::swap(current_exp, next_exp);
      std::swap(current_dir, next_dir);
      next_exp.clear();
      next_dir.clear();
    };

    return true;
  };

} _;
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
