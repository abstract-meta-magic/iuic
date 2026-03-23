// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying:utils.tree.dfs;
import :utils.tree.decl;
import :erasure;

namespace iuic::utils::tree {
struct dfs_hierarchy_node_t {
  using index_t = std::size_t;
  static constexpr index_t npos = std::numeric_limits<index_t>::max();
  static constexpr index_t root = npos - 1;
  index_t parent{npos};
  index_t child{npos};
  index_t left{npos};
  index_t right{npos};
};
}; // namespace iuic::utils::tree

export namespace iuic::utils::tree {

template <erasure::is_pure_type T> struct flat_dfs_type {

  // ---------- ITERATORS ----------
  using base_iterator = base_iterator<flat_dfs_type>;
  using access_iterator = access_iterator<flat_dfs_type>;
  using const_access_iterator = const_access_iterator<flat_dfs_type>;
  using root_iterator = root_iterator<flat_dfs_type>;
  using sibling_iterator = sibling_iterator<flat_dfs_type>;
  friend base_iterator;
  friend access_iterator;
  friend const_access_iterator;
  friend root_iterator;
  friend sibling_iterator;
  // c/m semantic
  // ---------- ITERATORS ----------

  template <typename U>
  friend auto tree::childs_of(tree::base_iterator<flat_dfs_type<U>> it);

  template <typename U>
  friend auto parent_of(tree::base_iterator<flat_dfs_type<U>> it);

  base_iterator root() { return {dfs_hierarchy_node_t::root, this}; };

  base_iterator begin() { return {0, this}; };

  base_iterator at(std::size_t index) { return {index, *this}; }

  std::span<T> flat() { return data__; };

  std::span<const T> flat() const { return data__; };

private:
  std::vector<T> data__;
  std::vector<dfs_hierarchy_node_t> hierarchy__;
};

template <typename T> struct base_iterator<flat_dfs_type<T>> {
  using container_t = flat_dfs_type<T>;
  using value_t = T;
  using lvalue_t = T &;
  using rvalue_t = T &&;
  using pointer_t = T *;
  using index_t = dfs_hierarchy_node_t::index_t;

  bool valid() const {
    return owner && self < owner->data__.size() && not owner->data__.empty();
  };

  operator bool() { return valid(); };

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

  base_iterator(index_t self_, container_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator() : self{dfs_hierarchy_node_t::npos}, owner{nullptr} {};

  template <typename U>
  friend auto tree::childs_of(tree::base_iterator<flat_dfs_type<U>> it);

  template <typename U>
  friend auto parent_of(tree::base_iterator<flat_dfs_type<U>> it);

protected:
  index_t self{dfs_hierarchy_node_t::npos};
  container_t *owner{nullptr};
};

template <typename T>
struct access_iterator<flat_dfs_type<T>> : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

  access_iterator(base::index_t index, base::container_t *owner)
      : base{index, owner} {}

  access_iterator(base b) : base{b} {}

  access_iterator() {}

  base::lvalue_t operator*() { return base::owner->data__[base::self]; };

  base::pointer_t operator->() {
    return std::addressof(base::owner->data__[base::self]);
  }
};

template <typename T>
struct const_access_iterator<flat_dfs_type<T>>
    : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

  const_access_iterator(base::index_t index, base::container_t *owner)
      : base{index, owner} {}

  const_access_iterator(base b) : base{b} {}

  const_access_iterator() {}

  const base::lvalue_t operator*() { return base::owner->data__[base::self]; };

  const base::pointer_t operator->() {
    return std::addressof(base::owner->data__[base::self]);
  }
};

template <typename T>
struct root_iterator<flat_dfs_type<T>> : access_iterator<flat_dfs_type<T>> {
  using base = access_iterator<flat_dfs_type<T>>;

  root_iterator(base::index_t index, base::container_t *owner)
      : base{index, owner} {}

  root_iterator(base::base b) : base{b} {}

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

template <typename T>
struct sibling_iterator<flat_dfs_type<T>> : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

  sibling_iterator(base::index_t index, base::container_t *owner)
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

// HINTS

template <typename T>
access_iterator(base_iterator<flat_dfs_type<T>>)
    -> access_iterator<flat_dfs_type<T>>;

template <typename T>
const_access_iterator(base_iterator<flat_dfs_type<T>>)
    -> const_access_iterator<flat_dfs_type<T>>;

template <typename T>
root_iterator(base_iterator<flat_dfs_type<T>>)
    -> root_iterator<flat_dfs_type<T>>;

template <typename T>
sibling_iterator(base_iterator<flat_dfs_type<T>>)
    -> sibling_iterator<flat_dfs_type<T>>;

template <typename T> auto childs_of(tree::base_iterator<flat_dfs_type<T>> it) {
  if (it.valid()) {
    return sibling_iterator<flat_dfs_type<T>>{
        it.owner->hierarchy__[it.self].child, it.owner};
  } else if (it.self == dfs_hierarchy_node_t::root) {
    return sibling_iterator<flat_dfs_type<T>>{0, it.owner};
  } else {
    return sibling_iterator<flat_dfs_type<T>>{};
  };
};

template <typename T> auto parent_of(tree::base_iterator<flat_dfs_type<T>> it) {
  if (it.valid()) {
    return sibling_iterator<flat_dfs_type<T>>{
        it.owner->hierarchy__[it.self].parent, it.owner};
  } else {
    return sibling_iterator<flat_dfs_type<T>>{};
  };
};

template <typename T, typename U>
base_iterator<flat_dfs_type<T>> shift(base_iterator<flat_dfs_type<T>> lhs,
                                      base_iterator<flat_dfs_type<U>> rhs) {
  // UNSAFE
  // NEED TO CHECK HIERARCHY
  struct : decltype(lhs), decltype(rhs) {
    decltype(lhs) value() {
      return decltype(lhs){decltype(rhs)::self, decltype(lhs)::owner};
    };
  } shift{lhs, rhs};

  return shift.value();
};

template <typename T>
base_iterator<flat_dfs_type<T>> shift(base_iterator<flat_dfs_type<T>> lhs,
                                      base_iterator<flat_dfs_type<T>> rhs) {
  return rhs;
};

template <typename T> struct dfs_range_for<flat_dfs_type<T>> {
  struct iterator : access_iterator<flat_dfs_type<T>> {
    using base = access_iterator<flat_dfs_type<T>>;

    iterator(base::index_t index, base::container_t *owner)
        : base{index, owner} {}

    iterator &operator++() {
      if (base::valid()) {
        ++base::self;
      }
      return *this;
    };

    iterator operator++(int) {
      auto res = *this;
      ++*this;
      return res;
    };
  };

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  dfs_range_for(flat_dfs_type<T> &container)
      : begin_{0, std::addressof(container)} {};

private:
  iterator begin_;
};

template <typename T>
dfs_range_for(flat_dfs_type<T> &) -> dfs_range_for<flat_dfs_type<T>>;

void heh(flat_dfs_type<int> &c) {
  base_iterator it = c.begin();
  access_iterator ait{it};
  const_access_iterator cait{it};
  root_iterator rit{it};
  ++rit;
  rit++;
  sibling_iterator sit{it};
  sit++;
  ++sit;
  --sit;
  sit--;

  parent_of(it);
  childs_of(it);

  for (auto &&el : dfs_range_for{c}) {
  }
}
}; // namespace iuic::utils::tree
