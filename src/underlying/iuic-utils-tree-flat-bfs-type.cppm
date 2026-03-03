

export module iuic.underlying:utils.tree.bfs;
import :utils.tree.decl;

namespace iuic::utils::tree {

struct bfs_hierarchy_node_t {
  static constexpr auto npos = std::numeric_limits<std::size_t>::max();
  // abstract element on top hierarhy without access
  static constexpr auto root = npos - 1;
  using index_t = std::size_t;
  index_t parent{root};
  index_t ch_begin{npos};
  index_t ch_end{npos};
};
}; // namespace iuic::utils::tree

export namespace iuic::utils::tree {

template <erasure::as_pure_type T> struct flat_bfs_type {
  using value_t = T;
  using ref_t = T &;
  using cref_t = const T &;
  using index_t = bfs_hierarchy_node_t::index_t;
  static constexpr auto npos = bfs_hierarchy_node_t::npos;
  static constexpr auto root = bfs_hierarchy_node_t::root;

  // ---------- ITERATORS ----------
  using base_iterator = tree::base_iterator<flat_bfs_type>;
  using access_iterator = tree::access_iterator<flat_bfs_type>;
  using const_access_iterator = tree::const_access_iterator<flat_bfs_type>;
  using root_iterator = tree::root_iterator<flat_bfs_type>;
  using sibling_iterator = tree::sibling_iterator<flat_bfs_type>;
  using const_root_iterator = tree::const_root_iterator<flat_bfs_type>;
  using const_sibling_iterator = tree::const_sibling_iterator<flat_bfs_type>;
  friend base_iterator;
  friend access_iterator;
  friend const_access_iterator;
  friend root_iterator;
  friend const_root_iterator;
  friend sibling_iterator;
  friend const_sibling_iterator;
  // ---------- ITERATORS ----------

  std::span<T> flat();

  std::span<const T> flat() const;

  base_iterator begin() {
    if (data__.empty()) {
      return {};
    } else {
      return {0, this};
    }
  };

  base_iterator at(index_t i) {
    if (i < data__.size()) {
      return {i, this};
    } else {
      return {};
    }
  };

  auto reflect(std::invocable<const T &> auto &&mapper) const {
    using ftraits = typename decltype(erasure::func_type{mapper})::traits;

    if constexpr (std::same_as<typename ftraits::return_t, void>) {
      return flat_bfs_type{*this};
    } else {
      using type = typename std::remove_cvref_t<typename ftraits::return_t>;
      flat_bfs_type<type> tree;

      tree.hierarchy__ = hierarchy__;

      tree.data__.reserve(data__.size());
      auto begin = data__.begin();
      auto end = data__.end();
      auto insert = tree.data__.begin();

      for (auto &element : data__) {
        tree.data__.push_back(mapper(element));
      }

      return tree;
    }
  };

  flat_bfs_type() {};

  template <typename Other>
  flat_bfs_type(tree::copy_iterator<type::bfs, Other> it){};

  template <typename Other>
  flat_bfs_type(tree::move_iterator<type::bfs, Other> it) {
    using op = insert_op<type::bfs>;

    op op_{op::ins};

    // 4 base_iterators
    tree::base_iterator parents_begin{it};
    tree::base_iterator childs_begin{it};

    for (; it; it.advance(op_)) {
      switch (op_) {
      case op::ins: {
        break;
      }
      case op::sep: {
        break;
      }
      case op::deep: {
        break;
      }
      case op::end: {
        return;
      }
      }
    };
  };

private:
  template <erasure::as_pure_type S> friend struct flat_bfs_type;

  std::vector<T> data__;
  std::vector<bfs_hierarchy_node_t> hierarchy__;
};

/// ---------- ITERATORS ---------- ///

template <typename T> struct base_iterator<flat_bfs_type<T>> {
  using container_t = flat_bfs_type<T>;
  using value_t = T;
  using lvalue_t = T &;
  using rvalue_t = T &&;
  using pointer_t = T *;
  using index_t = container_t::index_t;

  bool valid() const {
    return owner && self < owner->data__.size() && not owner->data__.empty();
  };

  operator bool() const { return valid(); }

  bool operator==(const tree::sentinel<flat_bfs_type<T>> &) const {
    return *this;
  }

  base_iterator(index_t self_, container_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator() : self{container_t::npos}, owner{nullptr} {};

  template <typename U>
  friend auto tree::childs_of(tree::base_iterator<flat_bfs_type<U>> it);

  template <typename U>
  friend auto parent_of(tree::base_iterator<flat_bfs_type<U>> it);

protected:
  index_t self{container_t::npos};
  container_t *owner{nullptr};
};

template <typename T>
struct access_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  access_iterator(base::index_t self, base::container_t *owner)
      : base{self, owner} {}

  access_iterator() : base{} {};

  access_iterator(base b) : base{b} {};

  base::pointer_t operator->() { return &base::owner->data__[base::self]; }

  base::lvalue_t operator*() { return base::owner->data__[base::self]; }
};

template <typename T>
struct const_access_iterator<flat_bfs_type<T>>
    : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  const_access_iterator(base::index_t self, base::container_t *owner)
      : base{self, owner} {}

  const_access_iterator() : base{} {};

  const_access_iterator(access_iterator<T> b) : base{b} {};

  const_access_iterator(base b) : base{b} {};

  const T *operator->() { return &base::owner->data__[base::self]; }

  const T &operator*() { return base::owner->data__[base::self]; }
};

template <typename T>
struct root_iterator<flat_bfs_type<T>> : access_iterator<flat_bfs_type<T>> {
  using base = access_iterator<flat_bfs_type<T>>;

  root_iterator() : base{} {};

  root_iterator(base::base b) : base{b} {}

  root_iterator(base b) : base{b} {}

  root_iterator(base::index_t self, base::container_t *owner)
      : base{self, owner} {}

  root_iterator &operator++() {
    if (base::valid()) {
      auto &hnode = base::owner->hierarchy__[base::self];

      base::self = hnode.parent;
    }

    return *this;
  }

  root_iterator operator++(int) {
    if (base::valid()) {
      return {base::owner->hierarchy__[base::self].parent, base::owner};
    } else {
      return {};
    }

    return *this;
  }
};

template <typename T>
struct const_root_iterator<flat_bfs_type<T>>
    : const_access_iterator<flat_bfs_type<T>> {
  using base = const_access_iterator<flat_bfs_type<T>>;

  const_root_iterator() : base{} {};

  const_root_iterator(base::base b) : base{b} {}

  const_root_iterator(base b) : base{b} {}

  const_root_iterator(root_iterator<T> b) : base{b} {}

  const_root_iterator(base::index_t self, base::container_t *owner)
      : base{self, owner} {}

  const_root_iterator &operator++() {
    if (base::valid()) {
      auto &hnode = base::owner->hierarchy__[base::self];

      base::self = hnode.parent;
    }

    return *this;
  }

  const_root_iterator operator++(int) {
    if (base::valid()) {
      return {base::owner->hierarchy__[base::self].parent, base::owner};
    } else {
      return {};
    }

    return *this;
  }
};

template <typename T>
struct sibling_iterator<flat_bfs_type<T>> : access_iterator<flat_bfs_type<T>> {
  using base = access_iterator<flat_bfs_type<T>>;

  sibling_iterator() : base{} {};

  sibling_iterator(base::base b) : base{b} {}

  sibling_iterator(base b) : base{b} {}

  sibling_iterator(base::index_t self, base::container_t *owner)
      : base{self, owner} {}

  sibling_iterator &operator++() {
    if (base::valid()) {
      base::self = base::ownel->hierarchy__[base::self].right;
    }
    return *this;
  }

  sibling_iterator &operator--() {
    if (base::valid()) {
      base::self = base::ownel->hierarchy__[base::self].left;
    }
    return *this;
  }

  sibling_iterator operator++(int) {
    if (base::valid()) {
      return {base::ownel->hierarchy__[base::self].right, base::oner};
    } else {
      return {};
    }
    return *this;
  }

  sibling_iterator operator--(int) {
    if (base::valid()) {
      return {base::ownel->hierarchy__[base::self].left, base::oner};
    } else {
      return {};
    }
  }
};

template <typename T>
struct const_sibling_iterator<flat_bfs_type<T>>
    : const_access_iterator<flat_bfs_type<T>> {
  using base = const_access_iterator<flat_bfs_type<T>>;

  const_sibling_iterator() : base{} {};

  const_sibling_iterator(base::base b) : base{b} {}

  const_sibling_iterator(base b) : base{b} {}

  const_sibling_iterator(sibling_iterator<T> b) : base{b} {}

  const_sibling_iterator(base::index_t self, base::container_t *owner)
      : base{self, owner} {}

  const_sibling_iterator &operator++() {
    if (base::valid()) {
      auto pnode =
          base::owner->hierarchy__[base::owner->hierarchy__[base::self].parent];

      if (pnode.ch_end < base::self) {
        ++base::self;
      } else {
        base::self = base::base::container_t::npos;
      }
    }
    return *this;
  }

  const_sibling_iterator &operator--() {
    if (base::valid()) {
      base::self = base::owner->hierarchy__[base::self].left;
    }
    return *this;
  }

  const_sibling_iterator operator++(int) {
    if (base::valid()) {
      return {base::ownel->hierarchy__[base::self].right, base::oner};
    } else {
      return {};
    }
    return *this;
  }

  const_sibling_iterator operator--(int) {
    if (this->valid()) {
      return {base::ownel->hierarchy__[base::self].left, base::oner};
    } else {
      return {};
    }
  }
};

template <typename T>
access_iterator(base_iterator<flat_bfs_type<T>> it)
    -> access_iterator<flat_bfs_type<T>>;

template <typename T>
const_access_iterator(base_iterator<flat_bfs_type<T>> it)
    -> const_access_iterator<flat_bfs_type<T>>;

template <typename T>
sibling_iterator(base_iterator<flat_bfs_type<T>> it)
    -> sibling_iterator<flat_bfs_type<T>>;

template <typename T>
root_iterator(base_iterator<flat_bfs_type<T>> it)
    -> root_iterator<flat_bfs_type<T>>;

template <typename T>
const_sibling_iterator(base_iterator<flat_bfs_type<T>> it)
    -> const_sibling_iterator<flat_bfs_type<T>>;

template <typename T>
const_root_iterator(base_iterator<flat_bfs_type<T>> it)
    -> const_root_iterator<flat_bfs_type<T>>;

template <typename T> auto childs_of(base_iterator<flat_bfs_type<T>> it) {

  if (it) {
    return sibling_iterator<flat_bfs_type<T>>{
        it.owner->hierarchy__[it.self].ch_begin, it.owner};
  } else {
    return sibling_iterator<flat_bfs_type<T>>{flat_bfs_type<T>::npos, it.owner};
  }
}

template <typename T> auto siblings_of(base_iterator<flat_bfs_type<T>> it) {
  return sibling_iterator<flat_bfs_type<T>>{it};
}

template <typename T> auto parent_of(base_iterator<flat_bfs_type<T>> it) {
  if (it) {
    return base_iterator<flat_bfs_type<T>>{
        it.owner->hierarchy__[it.self].parent, it.owner};
  } else {
    return base_iterator<flat_bfs_type<T>>{};
  }
}

template <typename T, typename U>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> lhs,
                                      base_iterator<flat_bfs_type<U>> rhs) {
  // UNSAFE
  // NEED TO CHECK HIERARCHY
  struct shift_iterator : decltype(lhs), decltype(rhs) {
    operator decltype(lhs)() {
      return decltype(lhs){decltype(rhs)::self, decltype(lhs)::owner};
    };
  } shift{lhs, rhs};

  return shift;
};

template <typename T>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> lhs,
                                      base_iterator<flat_bfs_type<T>> rhs) {
  return rhs;
};

} // namespace iuic::utils::tree
