

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
  static constexpr auto root_index = bfs_hierarchy_node_t::root;

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

  // ----------- RANGES ------------
  friend bfs_range_for<flat_bfs_type>;
  friend reverse_bfs_range_for<flat_bfs_type>;
  // ----------- RANGES ------------

  std::span<T> flat();

  std::span<const T> flat() const;

  base_iterator root() { return {root_index, this}; };

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
    std::size_t parent{root_index}, child{0};

    for (; it; it.advance(op_)) {
      switch (op_) {
      case op::ins: {
        data__.push_back(it.get());
        hierarchy__.push_back({.parent = parent});
        break;
      }
      case op::sep: {
        if (parent != root_index) {
          auto &ph_node = hierarchy__[parent];
          ph_node.ch_begin = child;
          ph_node.ch_end = child = hierarchy__.size();
        }
        ++parent;
        break;
      }
      case op::deep: {
        if (parent == root_index) {
          parent = 0;
          child = hierarchy__.size();
        }
        break;
      }
      case op::end: {
        for (auto &hnode : hierarchy__) {
          std::println("p:{} , chb :{} , che :{}", hnode.parent, hnode.ch_begin,
                       hnode.ch_end);
        }
        return;
      }
      }
    };
  };

private:
  template <erasure::as_pure_type S> friend struct flat_bfs_type;

  friend auto tree::childs_of(base_iterator it);
  friend auto tree::parent_of(base_iterator it);

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
      if (base::owner->hierarchy__[base::self + 1].parent ==
          base::owner->hierarchy__[base::self].parent) {
        ++base::self;
      } else {
        base::self = base::container_t::npos;
      };
    }
    return *this;
  }

  sibling_iterator &operator--() {
    if (base::valid()) {
      if (base::owner->hierarchy__[base::self - 1].parent ==
          base::owner->hierarchy__[base::self].parent) {
        --base::self;
      } else {
        base::self = base::container_t::npos;
      };
    }
    return *this;
  }

  sibling_iterator operator++(int) {
    // IMPL
    return {};
  }

  sibling_iterator operator--(int) {
    // IMPL
    return {};
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
  } else if (it.self == flat_bfs_type<T>::root_index) {
    return sibling_iterator<flat_bfs_type<T>>{0, it.owner};
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

template <typename T> struct reverse_bfs_range_for<flat_bfs_type<T>> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::index_t i, typename base::container_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return --base::self, *this; };
  };

  reverse_bfs_range_for(const flat_bfs_type<T> &container)
      : begin_{container.hierarchy__.size() - 1,
               const_cast<typename iterator::base::container_t *>(
                   std::addressof(container))} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T> struct bfs_range_for<flat_bfs_type<T>> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::index_t i, typename base::container_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };
  };

  bfs_range_for(const flat_bfs_type<T> &container)
      : begin_{0, const_cast<typename iterator::base::container_t *>(
                      std::addressof(container))} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T>
bfs_range_for(const flat_bfs_type<T> &) -> bfs_range_for<flat_bfs_type<T>>;

template <typename T>
reverse_bfs_range_for(const flat_bfs_type<T> &)
    -> reverse_bfs_range_for<flat_bfs_type<T>>;

} // namespace iuic::utils::tree
