// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:flat.hierarchy.bfs;
import std;
import :decl;

namespace iuic::tree::hierarchy {

struct bfs_base {
  using index_t = std::size_t;
  struct node_t {
    static constexpr auto npos = std::numeric_limits<std::size_t>::max();
    // abstract element on top hierarhy without access
    static constexpr auto root = npos - 1;
    index_t parent{root};
    index_t ch_begin{npos};
    index_t ch_end{npos};
  };

  std::vector<node_t> hierarchy__;
};
export struct bfs;
}; // namespace iuic::tree::hierarchy

export namespace iuic::tree {

template <> struct base_iterator<hierarchy::bfs> {
protected:
  using owner_t = hierarchy::bfs_base;

public:
  using container_t = hierarchy::bfs;
  using index_t = hierarchy::bfs_base::index_t;
  using value_t = index_t;
  using lvalue_t = index_t &;
  using rvalue_t = index_t &&;
  using pointer_t = index_t *;
  static constexpr auto npos = hierarchy::bfs_base::node_t::npos;
  static constexpr auto root_index = hierarchy::bfs_base::node_t::root;

  bool valid() const { return owner && self < owner->hierarchy__.size(); };

  bool is_root() const {
    return owner && self == hierarchy::bfs_base::node_t::root;
  };

  operator bool() const { return valid(); }

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

  base_iterator(index_t self_, owner_t *owner_) : self{self_}, owner{owner_} {}

  base_iterator() : self{hierarchy::bfs_base::node_t::npos}, owner{nullptr} {};

protected:
  std::size_t self;
  owner_t *owner;
};

}; // namespace iuic::tree

export namespace iuic::tree::hierarchy {

struct bfs : protected bfs_base {
  using base_iterator = base_iterator<bfs>;
  using root_iterator = root_iterator<bfs>;
  using sibling_iterator = sibling_iterator<bfs>;

  base_iterator begin() const {
    if (this->hierarchy__.empty()) {
      return {};
    } else {
      return {0, const_cast<bfs *>(this)};
    };
  };

  base_iterator root() const {
    if (this->hierarchy__.empty()) {
      return {};
    } else {
      return {bfs_base::node_t::root, const_cast<bfs *>(this)};
    };
  };

  bfs() {};

  template <typename Other> explicit bfs(tree::base_iterator<Other> it) {
    using other_base_iterator = decltype(it);

    std::vector<other_base_iterator> cur{};
    std::vector<other_base_iterator> next{};

    for (auto ch : iterator_range_for{childs_of(it)}) {
      cur.push_back(ch);
    }
    cur.push_back({}); // sep

    std::size_t index{0}, parent{tree::hierarchy::bfs_base::node_t::root},
        ch_begin{0}, ch_count{0}, counter{0};
    bool deep{false};

    for (;; ++index) {
      if (index >= cur.size()) {
        // deep
        if (deep) {
          cur = std::move(next);
          next.clear();
          index = 0;
          deep = false;
        } else {
          std::println("move fuck : {}", counter);
          return;
        }
      }

      if (not cur[index].valid()) {
        // sep
        if (parent != node_t::root) {
          auto &hnode = this->hierarchy__[parent];
          hnode.ch_begin = ch_begin;
          hnode.ch_end = this->hierarchy__.size();
          ++parent;
        } else {
          parent = 0;
        }
        ch_begin = this->hierarchy__.size();
        continue;
      }

      // insert
      this->hierarchy__.push_back({});
      this->hierarchy__.back().parent = parent;
      ++counter;

      for (auto ch : tree::iterator_range_for{childs_of(cur[index])}) {
        next.push_back(ch);
      }

      if (not deep && not next.empty()) {
        deep = next.back().valid();
      }

      next.push_back({}); // sep
    }
  };
};
}; // namespace iuic::tree::hierarchy

export namespace iuic::tree {

template <>
struct root_iterator<hierarchy::bfs> : base_iterator<hierarchy::bfs> {

  using base = base_iterator<hierarchy::bfs>;

  root_iterator() : base{} {};

  root_iterator(base b) : base{b} {}

  root_iterator(hierarchy::bfs_base::index_t self, base::owner_t *owner)
      : base{self, owner} {}

  root_iterator &operator++() {
    if (this->valid()) {
      auto &hnode = this->owner->hierarchy__[this->self];

      this->self = hnode.parent;
    }

    return *this;
  }

  root_iterator operator++(int) {
    if (this->valid()) {
      return {this->owner->hierarchy__[this->self].parent, this->owner};
    } else {
      return {};
    }

    return *this;
  }
};

template <>
struct sibling_iterator<hierarchy::bfs> : base_iterator<hierarchy::bfs> {
  using base = base_iterator<hierarchy::bfs>;

  sibling_iterator() : base{} {};

  sibling_iterator(base b) : base{b} {}

  sibling_iterator(hierarchy::bfs_base::index_t self, base::owner_t *owner)
      : base{self, owner} {}

  sibling_iterator &operator++() {
    if (this->valid()) {
      auto next = this->self + 1;
      auto &hierarchy = this->owner->hierarchy__;
      if (hierarchy.size() > next &&
          hierarchy[this->self].parent == hierarchy[next].parent) {
        ++this->self;
      } else {
        this->self = hierarchy::bfs_base::node_t::npos;
      };
    }
    return *this;
  }

  sibling_iterator &operator--() {
    if (this->valid()) {
      auto &hierarchy = this->owner->hierarchy__;
      if (this->self != 0 &&
          hierarchy[this->self].parent == hierarchy[this->self - 1].parent) {
        --this->self;
      } else {
        this->self = hierarchy::bfs_base::node_t::npos;
      };
    }
    return *this;
  }

  sibling_iterator operator++(int) {
    auto res = *this;
    ++*this;
    return res;
  }

  sibling_iterator operator--(int) {
    auto res = *this;
    --*this;
    return res;
  }
};

root_iterator(base_iterator<hierarchy::bfs>) -> root_iterator<hierarchy::bfs>;
sibling_iterator(base_iterator<hierarchy::bfs>)
    -> sibling_iterator<hierarchy::bfs>;

template <> auto childs_of(base_iterator<hierarchy::bfs> it) {
  struct : base_iterator<hierarchy::bfs> {
    using base = decltype(it);
    sibling_iterator<hierarchy::bfs> find() {
      if (this->valid()) {
        return base{this->owner->hierarchy__[this->self].ch_begin, this->owner};
      } else if (this->self == hierarchy::bfs_base::node_t::root) {
        return base{0, this->owner};
      } else {
        return base{hierarchy::bfs_base::node_t::npos, this->owner};
      }
    }
  } child_search{it};
  return child_search.find();
}

template <template <typename> typename Iterator>
struct bfs_iterator_range_for<hierarchy::bfs, Iterator> {
  struct iterator : base_iterator<hierarchy::bfs> {
    using base = base_iterator<hierarchy::bfs>;
    using iterator_t = Iterator<hierarchy::bfs>;

    iterator(base b) : base{b} {};

    iterator(hierarchy::bfs_base::index_t i, typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    iterator_t operator*() { return base{*this}; };
  };

  bfs_iterator_range_for(base_iterator<hierarchy::bfs> it,
                         iterator_type<Iterator>)
      : begin_{it} {}

  bfs_iterator_range_for(const hierarchy::bfs &container)
      : begin_{container.begin()} {}

  bfs_iterator_range_for(const hierarchy::bfs &container,
                         iterator_type<Iterator>)
      : begin_{container.begin()} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <template <typename> typename Iterator>
bfs_iterator_range_for(const hierarchy::bfs &, iterator_type<Iterator>)
    -> bfs_iterator_range_for<hierarchy::bfs, Iterator>;

template <template <typename> typename Iterator>
bfs_iterator_range_for(base_iterator<hierarchy::bfs>, iterator_type<Iterator>)
    -> bfs_iterator_range_for<hierarchy::bfs, Iterator>;

}; // namespace iuic::tree
