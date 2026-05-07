// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:flat.dfs;
import iuic.underlying.utils;
import :decl;
export import :flat.hierarchy.dfs;

//////////////////////////////////////////////////////////////
/// DECL//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

export namespace iuic::tree {

template <erasure::is_pure_type T>
struct flat_dfs_type_base : hierarchy::dfs_base {
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
  using hierarchy = hierarchy::dfs_base;

public: // operations
  auto reflect(std::invocable<const T &> auto &&mapper) const;

public: // access
  std::span<T> flat();

  std::span<const T> flat() const;

  base_iterator root() const;

  base_iterator begin() const;

  base_iterator end() const;

  base_iterator at(std::size_t index);

public: // ctor's
  explicit flat_dfs_type() noexcept;

  template <typename Other> explicit flat_dfs_type(move_iterator<Other> it);

  template <typename Other> explicit flat_dfs_type(copy_iterator<Other> it);

private:
  template <erasure::is_pure_type S> friend struct flat_dfs_type;
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

public: // operations
  bool is_root() const {
    return owner && self == container_t::hierarchy::node_t::root;
  };

  bool valid() const {
    return owner && self < owner->data__.size() && not owner->data__.empty();
  };

public: // compares
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

public: // ctor's
  base_iterator(container_t::hierarchy::index_t self_, owner_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator()
      : self{container_t::hierarchy::node_t::npos}, owner{nullptr} {};

public: // cast
  operator bool() const { return valid(); };

  operator base_iterator<hierarchy::dfs>() const { return {self, owner}; };

protected:
  container_t::hierarchy::index_t self{container_t::hierarchy::node_t::npos};
  owner_t *owner{nullptr};
};

template <typename T>
struct access_iterator<flat_dfs_type<T>> : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

public: // operators
  base::lvalue_t operator*() { return this->owner->data__[base::self]; };

  base::pointer_t operator->() {
    return std::addressof(this->owner->data__[base::self]);
  }

public: // ctor's
  access_iterator(base::container_t::hierarchy::index_t index,
                  base::owner_t *owner)
      : base{index, owner} {}

  explicit access_iterator(base b) : base{b} {}

  explicit access_iterator() {}
};

template <typename T>
struct const_access_iterator<flat_dfs_type<T>>
    : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

public: // operators
  const base::lvalue_t operator*() { return this->owner->data__[base::self]; };

  const base::pointer_t operator->() {
    return std::addressof(this->owner->data__[base::self]);
  }

public: // ctor's
  const_access_iterator(base::container_t::hierarchy::index_t index,
                        base::owner_t *owner)
      : base{index, owner} {}

  explicit const_access_iterator(base b) : base{b} {}

  explicit const_access_iterator() {}
};

template <typename T>
struct root_iterator<flat_dfs_type<T>> : access_iterator<flat_dfs_type<T>> {
  using base = access_iterator<flat_dfs_type<T>>;

public: // operators
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

public: // ctor's
  root_iterator(base::container_t::hierarchy::index_t index,
                base::owner_t *owner)
      : base{index, owner} {}

  explicit root_iterator(base::base b) : base{b} {}

  explicit root_iterator() {}
};

template <typename T>
struct sibling_iterator<flat_dfs_type<T>> : base_iterator<flat_dfs_type<T>> {
  using base = base_iterator<flat_dfs_type<T>>;

public: // operators
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

public: // ctor's
  sibling_iterator(base::container_t::hierarchy::index_t index,
                   base::owner_t *owner)
      : base{index, owner} {}

  explicit sibling_iterator(base b) : base{b} {}

  explicit sibling_iterator() {}
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
  struct : decltype(it) {
    using base = decltype(it);
    sibling_iterator<flat_dfs_type<T>> find() {
      if (this->valid()) {
        return sibling_iterator<flat_dfs_type<T>>{
            this->owner->hierarchy__[this->self].child, this->owner};
      } else if (this->self == base::container_t::hierarchy::node_t::root) {
        return sibling_iterator<flat_dfs_type<T>>{0, this->owner};
      } else {
        return sibling_iterator<flat_dfs_type<T>>{};
      };
    };
  } child_search{it};
  return child_search.find();
};

//////////////////////////////////////////////////////////////
/// IMPL//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/////////////////////////
// ::OPEROTION
template <erasure::is_pure_type T>
auto flat_dfs_type<T>::reflect(std::invocable<const T &> auto &&mapper) const {
  using ftraits = typename decltype(erasure::func_type{mapper})::traits;

  if constexpr (std::same_as<typename ftraits::return_t, void>) {
    return flat_dfs_type{*this};
  } else {
    using type = typename std::remove_cvref_t<typename ftraits::return_t>;
    flat_dfs_type<type> tree;

    tree.hierarchy__ = this->hierarchy__;

    tree.data__.reserve(this->data__.size());
    auto begin = this->data__.begin();
    auto end = this->data__.end();
    auto insert = tree.data__.begin();

    for (auto &element : this->data__) {
      tree.data__.push_back(mapper(element));
    }

    return tree;
  }
};

/////////////////////////
// ::ACCESS

template <erasure::is_pure_type T> std::span<T> flat_dfs_type<T>::flat() {
  return this->data__;
};

template <erasure::is_pure_type T>
std::span<const T> flat_dfs_type<T>::flat() const {
  return this->data__;
};

template <erasure::is_pure_type T>
base_iterator<flat_dfs_type<T>> flat_dfs_type<T>::root() const {
  return {hierarchy::node_t::root, const_cast<flat_dfs_type *>(this)};
};

template <erasure::is_pure_type T>
base_iterator<flat_dfs_type<T>> flat_dfs_type<T>::begin() const {
  return {0, const_cast<flat_dfs_type *>(this)};
};

template <erasure::is_pure_type T>
base_iterator<flat_dfs_type<T>> flat_dfs_type<T>::end() const {
  return {this->hierarchy__.size() - 1, const_cast<flat_dfs_type *>(this)};
};

template <erasure::is_pure_type T>
base_iterator<flat_dfs_type<T>> flat_dfs_type<T>::at(std::size_t index) {
  return {index, const_cast<flat_dfs_type *>(this)};
}

/////////////////////////
// ::CTOR
template <erasure::is_pure_type T>
flat_dfs_type<T>::flat_dfs_type() noexcept {};

template <erasure::is_pure_type T>
template <typename Other>
flat_dfs_type<T>::flat_dfs_type(move_iterator<Other> it) {
  using other_base_iterator = decltype(it)::base;
  struct ctor_iterator : tree::base_iterator<tree::hierarchy::dfs> {
    using base = tree::base_iterator<tree::hierarchy::dfs>;
    ctor_iterator(hierarchy::index_t self, tree::hierarchy::dfs_base *owner)
        : base{self, owner} {};

    void push() { this->self = this->owner->hierarchy__[this->self].child; };

    void shift() { this->self = this->owner->hierarchy__[this->self].right; };

    void pop() { this->self = this->owner->hierarchy__[this->self].parent; };

    hierarchy::index_t get_self() { return this->self; };

    hierarchy::node_t &get_node() {
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

  this->data__.push_back(*tree::move_iterator{it});
  this->hierarchy__.push_back({}); // FIRST|ROOT_CH_0

  for (; cur;) {
    if (auto ch{childs_of(cur)}) {
      // set data\hierarchy
      this->data__.push_back(*tree::move_iterator{ch});
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
          this->data__.push_back(*tree::move_iterator{sib});
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

template <erasure::is_pure_type T>
template <typename Other>
flat_dfs_type<T>::flat_dfs_type(copy_iterator<Other> it) {

  using other_base_iterator = decltype(it)::base;
  struct ctor_iterator : tree::base_iterator<tree::hierarchy::dfs> {
    using base = tree::base_iterator<tree::hierarchy::dfs>;
    ctor_iterator(hierarchy::index_t self, tree::hierarchy::dfs_base *owner)
        : base{self, owner} {};

    void push() { this->self = this->owner->hierarchy__[this->self].child; };

    void shift() { this->self = this->owner->hierarchy__[this->self].right; };

    void pop() { this->self = this->owner->hierarchy__[this->self].parent; };

    hierarchy::index_t get_self() { return this->self; };

    hierarchy::node_t &get_node() {
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

  this->data__.push_back(*tree::copy_iterator{it});
  this->hierarchy__.push_back({}); // FIRST|ROOT_CH_0

  for (; cur;) {
    if (auto ch{childs_of(cur)}) {
      // set data\hierarchy
      this->data__.push_back(*tree::copy_iterator{ch});
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
          this->data__.push_back(*tree::copy_iterator{sib});
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
