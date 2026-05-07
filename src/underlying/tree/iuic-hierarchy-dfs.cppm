// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:flat.hierarchy.dfs;
import std;
import :decl;

//////////////////////////////////////////////////////////////
/// DECL//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

export namespace iuic::tree::tag {
struct dfs : tag_qualifier__ {};
}; // namespace iuic::tree::tag

namespace iuic::tree::hierarchy {
struct dfs_base {
  using index_t = std::size_t;
  struct node_t {
    static constexpr index_t npos = std::numeric_limits<index_t>::max();
    static constexpr index_t root = npos - 1;
    index_t parent{npos};
    index_t child{npos}; // ch_begin also self + 1 \ need ch_end
    index_t left{npos};
    index_t right{npos};
  };

  std::vector<node_t> hierarchy__;
};

export struct dfs;
}; // namespace iuic::tree::hierarchy

export namespace iuic::tree {
template <> struct base_iterator<hierarchy::dfs> {
protected:
  using owner_t = hierarchy::dfs_base;

public:
  using container_t = hierarchy::dfs;
  using value_t = hierarchy::dfs_base::index_t;
  using lvalue_t = value_t &;
  using rvalue_t = value_t &&;
  using pointer_t = value_t *;

  bool valid() const {
    return owner ? owner->hierarchy__.size() > self : false;
  };

  bool is_root() {
    return owner ? self == hierarchy::dfs_base::node_t::root : false;
  };

  operator bool() const { return valid(); };

  template <typename D>
    requires std::is_base_of_v<base_iterator, D>
  bool operator==(const tree::sentinel<D> &) const {
    return not valid();
  }

  template <typename D>
    requires std::is_base_of_v<base_iterator, D>
  bool operator!=(const tree::sentinel<D> &) const {
    return valid();
  }

  base_iterator(hierarchy::dfs_base::index_t self_, owner_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator() : self{hierarchy::dfs_base::node_t::npos}, owner{nullptr} {};

protected:
  hierarchy::dfs_base::index_t self;
  owner_t *owner;
};
}; // namespace iuic::tree

export namespace iuic::tree::hierarchy {

struct dfs : protected dfs_base {
  using base_iterator = base_iterator<dfs>;
  using root_iterator = root_iterator<dfs>;
  using sibling_iterator = sibling_iterator<dfs>;
  using hierarchy = dfs_base;

  base_iterator root() const {
    return {dfs_base::node_t::root, const_cast<dfs *>(this)};
  };

  base_iterator begin() const { return {0, const_cast<dfs *>(this)}; };

  // overflow - OK
  base_iterator end() const {
    return {this->hierarchy__.size() - 1, const_cast<dfs *>(this)};
  };

  base_iterator at(hierarchy::dfs_base::index_t index) const {
    return {index, const_cast<dfs *>(this)};
  };

  template <typename Other> explicit dfs(tree::base_iterator<Other> it);
};
} // namespace iuic::tree::hierarchy

export namespace iuic::tree {

template <>
struct root_iterator<hierarchy::dfs> : base_iterator<hierarchy::dfs> {
  using base = base_iterator<hierarchy::dfs>;

  root_iterator(hierarchy::dfs_base::index_t index, base::owner_t *owner)
      : base{index, owner} {}

  root_iterator(base b) : base{b} {}

  root_iterator() {}

  root_iterator &operator++() {
    if (base::valid()) {
      base::self = base::owner->hierarchy__[base::self].parent;
    }
    return *this;
  }

  root_iterator operator++(int) {
    auto res = *this;
    ++*this;
    return res;
  };
};

template <>
struct sibling_iterator<hierarchy::dfs> : base_iterator<hierarchy::dfs> {
  using base = base_iterator<hierarchy::dfs>;

  sibling_iterator(hierarchy::dfs_base::index_t index, base::owner_t *owner)
      : base{index, owner} {}

  sibling_iterator(base b) : base{b} {}

  sibling_iterator() {}

  sibling_iterator &operator++() {
    if (base::valid()) {
      base::self = base::owner->hierarchy__[base::self].right;
    }
    return *this;
  };

  sibling_iterator &operator--() {
    if (base::valid()) {
      base::self = base::owner->hierarchy__[base::self].left;
    }
    return *this;
  };

  sibling_iterator operator++(int) {
    auto res = *this;
    ++*this;
    return res;
  };

  sibling_iterator operator--(int) {
    auto res = *this;
    ++*this;
    return res;
  };
};

root_iterator(base_iterator<hierarchy::dfs>) -> root_iterator<hierarchy::dfs>;

sibling_iterator(base_iterator<hierarchy::dfs>)
    -> sibling_iterator<hierarchy::dfs>;

template <typename T> auto childs_of(base_iterator<hierarchy::dfs> it) {
  struct : decltype(it) {
    using base = decltype(it);
    sibling_iterator<hierarchy::dfs> find() {
      if (this->valid()) {
        return {this->owner->hierarchy__[this->self].ch_begin, this->owner};
      } else if (this->is_root()) {
        return {0, this->owner};
      } else {
        return {};
      }
    };
  } child_search{it};

  return child_search.find();
};
//////////////////////////////////////////////////////////////
/// IMPL//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/////////////////////////
// ::CTOR

template <typename Other> hierarchy::dfs::dfs(tree::base_iterator<Other> it) {
  using other_base_iterator = decltype(it);
  struct ctor_iterator : tree::base_iterator<tree::hierarchy::dfs> {
    using base = tree::base_iterator<tree::hierarchy::dfs>;
    ctor_iterator(hierarchy::dfs_base::index_t self,
                  tree::hierarchy::dfs_base *owner)
        : base{self, owner} {};

    void push() { this->self = this->owner->hierarchy__[this->self].child; };

    void shift() { this->self = this->owner->hierarchy__[this->self].right; };

    void pop() { this->self = this->owner->hierarchy__[this->self].parent; };

    hierarchy::dfs_base::index_t get_self() { return this->self; };

    hierarchy::dfs_base::node_t &get_node() {
      return this->owner->hierarchy__[this->self];
    };
  };

  other_base_iterator cur;

  ctor_iterator cit{0, this};

  if (it.is_root()) {
    cur = childs_of(it);
  } else if (not it) {
    return; // invalid iterator
  } else {
    cur = it;
  }

  this->hierarchy__.push_back({}); // FIRST|ROOT_CH_0

  for (; cur;) {
    if (auto ch{childs_of(cur)}) {
      // set data\hierarchy
      cit.get_node().child = this->hierarchy__.size();
      this->hierarchy__.push_back({.parent = cit.get_self()}); // do job
      cur = ch;
      cit.push();
    } else {
      auto next = tree::root_iterator{cur};
      for (;;) {
        if (not next || next == it) {
          return; // finish
        } else if (auto sib = ++tree::sibling_iterator{next}) {
          // set data\hierarchy
          auto &prev_node = cit.get_node();
          prev_node.right = this->hierarchy__.size();
          this->hierarchy__.push_back({
              .parent = prev_node.parent,
              .left = cit.get_self(),
          }); // do job
          cur = sib;
          cit.shift();
          break;
        } else {
          ++next;
          cit.pop();
        }
      }
    }
  };
};
}; // namespace iuic::tree
