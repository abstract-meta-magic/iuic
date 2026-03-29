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
  index_t child{npos}; // ch_begin also self + 1 \ need ch_end
  index_t left{npos};
  index_t right{npos};
};

template <typename T> struct hierarchy_base;
}; // namespace iuic::utils::tree

// HIERARCHY
export namespace iuic::utils::tree {

template <erasure::is_pure_type T> struct flat_dfs_type_base;

template <erasure::is_pure_type T> struct flat_dfs_type;

template <typename T> struct hierarchy_base<flat_dfs_type<T>> {
  std::vector<dfs_hierarchy_node_t> hierarchy__;
};

template <typename T>
struct hierarchy<flat_dfs_type<T>> : hierarchy_base<flat_dfs_type<T>> {
  using base_iterator = base_iterator<hierarchy>;
  using root_iterator = root_iterator<hierarchy>;
  using sibling_iterator = sibling_iterator<hierarchy>;

  base_iterator root() const {
    return {dfs_hierarchy_node_t::root, const_cast<hierarchy *>(this)};
  };

  base_iterator begin() const { return {0, const_cast<hierarchy *>(this)}; };

  // overflow - OK
  base_iterator end() const {
    return {this->hierarchy__.size() - 1, const_cast<hierarchy *>(this)};
  };

  base_iterator at(dfs_hierarchy_node_t::index_t index) const {
    return {index, const_cast<hierarchy *>(this)};
  };

  template <typename Other>
  hierarchy(tree::base_iterator<Other> it){

  };
};

template <typename T> struct base_iterator<hierarchy<flat_dfs_type<T>>> {
protected:
  using owner_t = hierarchy_base<flat_dfs_type<T>>;

public:
  using container_t = hierarchy<flat_dfs_type<T>>;
  using value_t = dfs_hierarchy_node_t::index_t;
  using lvalue_t = value_t &;
  using rvalue_t = value_t &&;
  using pointer_t = value_t *;

  bool valid() const {
    return owner ? owner->hierarchy__.size() > self : false;
  };

  bool is_root() { return owner ? self == dfs_hierarchy_node_t::root : false; };

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

  base_iterator(dfs_hierarchy_node_t::index_t self_, owner_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator() : self{dfs_hierarchy_node_t::npos}, owner{nullptr} {};

protected:
  dfs_hierarchy_node_t::index_t self;
  owner_t *owner;
};

template <typename T>
struct root_iterator<hierarchy<flat_dfs_type<T>>>
    : base_iterator<hierarchy<flat_dfs_type<T>>> {
  using base = base_iterator<hierarchy<flat_dfs_type<T>>>;

  root_iterator(base::index_t index, base::owner_t *owner)
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

template <typename T>
root_iterator(base_iterator<hierarchy<flat_dfs_type<T>>>)
    -> root_iterator<hierarchy<flat_dfs_type<T>>>;

template <typename T>
struct sibling_iterator<hierarchy<flat_dfs_type<T>>>
    : base_iterator<hierarchy<flat_dfs_type<T>>> {
  using base = base_iterator<hierarchy<flat_dfs_type<T>>>;

  sibling_iterator(base::index_t index, base::owner_t *owner)
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

template <typename T>
sibling_iterator(base_iterator<hierarchy<flat_dfs_type<T>>>)
    -> sibling_iterator<hierarchy<flat_dfs_type<T>>>;

template <typename T>
auto childs_of(base_iterator<hierarchy<flat_dfs_type<T>>> it) {
  struct : base_iterator<hierarchy_base<flat_dfs_type<T>>> {
    sibling_iterator<hierarchy_base<flat_dfs_type<T>>> find() {
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
}; // namespace iuic::utils::tree

export namespace iuic::utils::tree {

template <erasure::is_pure_type T>
struct flat_dfs_type_base : hierarchy_base<flat_dfs_type<T>> {
  std::vector<T> data__;
};

template <erasure::is_pure_type T>
struct flat_dfs_type : protected flat_dfs_type_base<T> {
  using value_t = T;
  // ---------- ITERATORS ----------
  using base_iterator = base_iterator<flat_dfs_type>;
  using access_iterator = access_iterator<flat_dfs_type>;
  using const_access_iterator = const_access_iterator<flat_dfs_type>;
  using root_iterator = root_iterator<flat_dfs_type>;
  using sibling_iterator = sibling_iterator<flat_dfs_type>;
  // ---------- ITERATORS ----------

  base_iterator root() const {
    return {dfs_hierarchy_node_t::root, const_cast<flat_dfs_type *>(this)};
  };

  base_iterator begin() const {
    return {0, const_cast<flat_dfs_type *>(this)};
  };

  base_iterator end() const {
    return {this->hierarchy__.size() - 1, const_cast<flat_dfs_type *>(this)};
  };

  base_iterator at(std::size_t index) {
    return {index, const_cast<flat_dfs_type *>(this)};
  }

  std::span<T> flat() { return this->data__; };

  std::span<const T> flat() const { return this->data__; };

  template <typename Other>
  flat_dfs_type(move_iterator<Other> it){
      // TODO : body
  };

  template <typename Other>
  flat_dfs_type(copy_iterator<Other> it){
      // TODO : body
  };
};

template <typename T> struct base_iterator<flat_dfs_type<T>> {
protected:
  using owner_t = flat_dfs_type_base<T>;

public:
  using container_t = flat_dfs_type<T>;
  using value_t = T;
  using lvalue_t = T &;
  using rvalue_t = T &&;
  using pointer_t = T *;

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

  base_iterator(dfs_hierarchy_node_t::index_t self_, owner_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator() : self{dfs_hierarchy_node_t::npos}, owner{nullptr} {};

protected:
  dfs_hierarchy_node_t::index_t self{dfs_hierarchy_node_t::npos};
  owner_t *owner{nullptr};
};

template <typename T>
struct access_iterator<flat_dfs_type<T>> : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

  access_iterator(dfs_hierarchy_node_t::index_t index, base::owner_t *owner)
      : base{index, owner} {}

  access_iterator(base b) : base{b} {}

  access_iterator() {}

  base::lvalue_t operator*() { return this->owner->data__[base::self]; };

  base::pointer_t operator->() {
    return std::addressof(this->owner->data__[base::self]);
  }
};

template <typename T>
struct const_access_iterator<flat_dfs_type<T>>
    : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

  const_access_iterator(dfs_hierarchy_node_t::index_t index,
                        base::owner_t *owner)
      : base{index, owner} {}

  const_access_iterator(base b) : base{b} {}

  const_access_iterator() {}

  const base::lvalue_t operator*() { return this->owner->data__[base::self]; };

  const base::pointer_t operator->() {
    return std::addressof(this->owner->data__[base::self]);
  }
};

template <typename T>
struct root_iterator<flat_dfs_type<T>> : access_iterator<flat_dfs_type<T>> {
  using base = access_iterator<flat_dfs_type<T>>;

  root_iterator(dfs_hierarchy_node_t::index_t index, base::owner_t *owner)
      : base{index, owner} {}

  root_iterator(base::base b) : base{b} {}

  root_iterator() {}

  root_iterator &operator++() {
    if (this->valid()) {
      this->self = this->owner->hierarchy__[this->self].parent;
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

  sibling_iterator(dfs_hierarchy_node_t::index_t index, base::owner_t *owner)
      : base{index, owner} {}

  sibling_iterator(base b) : base{b} {}

  sibling_iterator() {}

  sibling_iterator &operator++() {
    if (this->valid()) {
      this->self = this->owner->hierarchy__[this->self].right;
    }
    return *this;
  };

  sibling_iterator &operator--() {
    if (this->valid()) {
      this->self = this->owner->hierarchy__[this->self].left;
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
  struct : base_iterator<flat_dfs_type<T>> {
    sibling_iterator<flat_dfs_type<T>> find() {
      if (this->valid()) {
        return sibling_iterator<flat_dfs_type<T>>{
            this->owner->hierarchy__[this->self].child, this->owner};
      } else if (this->self == dfs_hierarchy_node_t::root) {
        return sibling_iterator<flat_dfs_type<T>>{0, this->owner};
      } else {
        return sibling_iterator<flat_dfs_type<T>>{};
      };
    };
  } child_search{it};
  return child_search.find();
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

    iterator(dfs_hierarchy_node_t::index_t index, base::owner_t *owner)
        : base{index, owner} {}

    iterator(base b) : base{b} {}

    iterator &operator++() {
      if (this->valid()) { // this realy need ??
        ++this->self;
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

  dfs_range_for(flat_dfs_type<T> &container) : begin_{container.begin()} {};

private:
  iterator begin_;
};

template <typename T>
dfs_range_for(flat_dfs_type<T> &) -> dfs_range_for<flat_dfs_type<T>>;
}; // namespace iuic::utils::tree
