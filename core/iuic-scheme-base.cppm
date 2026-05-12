// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.base;
import std;
import iuic.underlying;
import iuic.text;
import iuic.layout;
import iuic.style;
import iuic.env;

namespace iuic::scheme {

struct sk_element {
  std::variant<const layout::frame *, const layout::text *> layout;
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
  std::span<const text::raw::token> text;
};

struct bp_element {
  units::ui::area area;
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
  std::span<const text::present::token> text;
  struct {
    enum flag { discarded = 0, virtualized, text, measure, applied, arrange };

    void set(flag f, bool value_ = true) { value.set(f, value_); };

    bool has(flag f) const { return value.test(f); }

  private:
    std::bitset<8> value{};
  } meta;
};

using sketch = tree::flat_bfs_type<sk_element>;

using blueprint = tree::flat_bfs_type<bp_element>;

export namespace iterators {

using base = tree::base_iterator<tree::hierarchy::bfs>;
using sentinel = tree::sentinel<base>;
using root = tree::root_iterator<tree::hierarchy::bfs>;
using sibling = tree::sibling_iterator<tree::hierarchy::bfs>;

sibling childs_of(base it) { return tree::childs_of(it); };

}; // namespace iterators

export namespace ranges {
// level_order

struct level_order {
  struct iterator : iterators::base {};

  iterator begin();

  tree::sentinel<iterator> end();
};

struct reverse_level_order {
  struct iterator : iterators::base {
    using base = iterators::base;
    iterator(base b) : base{b} {}
    iterator() {}

    iterator &operator*() { return *this; }

    iterator &operator++() {
      --self;
      return *this;
    };
  };

  reverse_level_order(iterators::base it) : begin_{find_begin__(it)} {};

  iterator begin() { return begin_; };

  tree::sentinel<iterator> end() { return {}; };

private:
  static iterator find_begin__(iterators::base it) {
    struct : iterators::base {
      iterator find() {
        return iterators::base{this->owner->hierarchy__.size() - 1,
                               this->owner};
      };
    } search_end{it};

    return search_end.find();
  };

private:
  iterator begin_;
};

struct postorder {
  struct iterator : iterators::base {
    using base = iterators::base;
    iterator(base b) : base{b} {}
    iterator() {}

    iterator &operator*() { return *this; }

    iterator &operator++() {
      throw std::runtime_error{"is boroken"};
      return *this;
    };
  };

  postorder(iterators::base it) : begin_{find_entry__(it)} {}

  iterator begin() { return begin_; };

  tree::sentinel<iterator> end() { return {}; };

private:
  static iterator find_entry__(iterators::base it) {
    struct : iterators::base {
      using base = iterator::base;
      iterator find() {
        if (owner && not owner->hierarchy__.empty()) {
          auto &h = owner->hierarchy__;
          index_t sel{0};

          for (; h[sel].ch_begin != npos;) {
            sel = h[sel].ch_begin;
          }
          return base{sel, owner};

        } else {
          return base{};
        }
      }
    } search_entry{it};
    return search_entry.find();
  };

private:
  iterator begin_;
};

struct preorder {
  struct iterator : iterators::base {};
  iterator begin() { throw std::runtime_error{"Not impl"}; };

  tree::sentinel<iterator> end() { return {}; };
};

// post_order
}; // namespace ranges

}; // namespace iuic::scheme
