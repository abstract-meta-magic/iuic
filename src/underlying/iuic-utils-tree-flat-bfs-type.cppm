// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying:utils.tree.bfs;
import :utils.tree.decl;
export import :utils.tree.hierarchy.bfs;

//////////////////////////////////////////////////////////////
/// DECL//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

export namespace iuic::utils::tree {

template <erasure::is_pure_type T>
struct flat_bfs_type_base : hierarchy::bfs_base {
  std::vector<T> data__;
};

template <erasure::is_pure_type T>
struct flat_bfs_type : protected flat_bfs_type_base<T> {
  using value_t = T;
  // ---------- ITERATORS ----------
  using base_iterator = tree::base_iterator<flat_bfs_type>;
  using access_iterator = tree::access_iterator<flat_bfs_type>;
  using const_access_iterator = tree::const_access_iterator<flat_bfs_type>;
  using root_iterator = tree::root_iterator<flat_bfs_type>;
  using sibling_iterator = tree::sibling_iterator<flat_bfs_type>;
  // ---------- ITERATORS ----------

  using hierarchy = hierarchy::bfs_base;

  std::span<T> flat();

  std::span<const T> flat() const;

  base_iterator root() const {
    return {hierarchy::node_t::root, const_cast<flat_bfs_type *>(this)};
  };

  base_iterator begin() const {
    if (this->data__.empty()) {
      return {};
    } else {
      return {0, const_cast<flat_bfs_type *>(this)};
    }
  };

  base_iterator end() const {
    if (this->data__.empty()) {
      return {};
    } else {
      return {this->data__.size() - 1, const_cast<flat_bfs_type *>(this)};
    }
  };

  base_iterator at(hierarchy::index_t i) {
    if (i < this->data__.size()) {
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

  flat_bfs_type() {};

  template <typename Other> flat_bfs_type(tree::copy_iterator<Other> it) {
    using op = insert_op<order_type::bfs>;
    using base_iterator = decltype(it)::base;
    using const_access_iterator = decltype(it);

    std::size_t index = 0, child = 0, parent = hierarchy::node_t::root;
    bool deep{false};
    std::vector<base_iterator> cur{};
    std::vector<base_iterator> next{};

    auto sep_ = [&]() {
      if (parent >= this->hierarchy__.size()) {
        child = this->hierarchy__.size();
      } else if (child != this->hierarchy__.size()) {
        auto &phnode = this->hierarchy__[parent];
        phnode.ch_begin = child;
        child = this->hierarchy__.size();
        phnode.ch_end = child;
      }

      parent == hierarchy::node_t::root ? parent = 0 : ++parent;
    };

    for (auto sit : iterator_range_for{tree::sibling_iterator{it}}) {
      cur.push_back(sit);
    }
    cur.push_back({}); // sep

    for (;; ++index) {
      if (index >= cur.size()) {
        if (deep) {
          cur = std::move(next);
          next.clear();
          index = 0;
          deep = false;
        } else {
          return; // end
        }
      } else if (not cur[index].valid()) {
        sep_();
        continue;
      }

      this->data__.push_back(*const_access_iterator{cur[index]});
      this->hierarchy__.push_back(
          hierarchy::node_t{.parent = parent}); // set parent

      for (auto ch : iterator_range_for{childs_of(cur[index])}) {
        next.push_back(ch);
      };

      if (not deep) {
        if (not next.empty()) {
          deep = next.back().valid();
        }
      }

      next.push_back({}); // sep
    };
  };

  template <typename Other> flat_bfs_type(tree::move_iterator<Other> it) {
    using op = insert_op<order_type::bfs>;
    using base_iterator = decltype(it)::base;
    using move_iterator = decltype(it);

    std::size_t index = 0, child = 0, parent = hierarchy::node_t::root;
    bool deep{false};
    std::vector<base_iterator> cur{};
    std::vector<base_iterator> next{};
    //

    auto sep_ = [&]() {
      if (parent >= this->hierarchy__.size()) {
        child = this->hierarchy__.size();
      } else if (child != this->hierarchy__.size()) {
        auto &phnode = this->hierarchy__[parent];
        phnode.ch_begin = child;
        child = this->hierarchy__.size();
        phnode.ch_end = child;
      }

      parent == hierarchy::node_t::root ? parent = 0 : ++parent;
    };

    for (auto sit : iterator_range_for{tree::sibling_iterator{it}}) {
      cur.push_back(sit);
    }
    cur.push_back({}); // sep

    for (;; ++index) {
      if (index >= cur.size()) {
        if (deep) {
          cur = std::move(next);
          next.clear();
          index = 0;
          deep = false;
        } else {
          return; // end
        }
      } else if (not cur[index].valid()) {
        sep_();
        continue;
      }

      this->data__.push_back(*move_iterator{cur[index]});
      this->hierarchy__.push_back(
          hierarchy::node_t{.parent = parent}); // set parent

      for (auto ch : iterator_range_for{childs_of(cur[index])}) {
        next.push_back(ch);
      };

      if (not deep) {
        if (not next.empty()) {
          deep = next.back().valid();
        }
      }

      next.push_back({}); // sep
    };
  };

private:
  template <erasure::is_pure_type S> friend struct flat_bfs_type;
};

/// ---------- ITERATORS ---------- ///

template <typename T> struct base_iterator<flat_bfs_type<T>> {
protected:
  using owner_t = flat_bfs_type_base<T>;

public:
  using container_t = flat_bfs_type<T>;
  using value_t = T;
  using lvalue_t = T &;
  using rvalue_t = T &&;
  using pointer_t = T *;

  bool valid() const {
    return owner && self < owner->data__.size() && not owner->data__.empty();
  };

  bool is_root() const {
    return owner && self == container_t::hierarchy::node_t::root;
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

  operator base_iterator<hierarchy::bfs>() const { return {self, owner}; }

  base_iterator(container_t::hierarchy::index_t self_, owner_t *owner_)
      : self{self_}, owner{owner_} {}

  base_iterator()
      : self{container_t::hierarchy::node_t::npos}, owner{nullptr} {};

  template <typename U>
  friend auto tree::childs_of(tree::base_iterator<flat_bfs_type<U>> it);

  template <typename U>
  friend auto parent_of(tree::base_iterator<flat_bfs_type<U>> it);

protected:
  container_t::hierarchy::index_t self{container_t::npos};
  flat_bfs_type_base<T> *owner{nullptr};
};

template <typename T>
struct access_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  access_iterator(base::container_t::hierarchy::index_t self,
                  base::owner_t *owner)
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

  const_access_iterator(base::container_t::hierarchy::index_t self,
                        base::owner_t *owner)
      : base{self, owner} {}

  const_access_iterator() : base{} {};

  const_access_iterator(base b) : base{b} {};

  const T *operator->() { return &base::owner->data__[base::self]; }

  const T &operator*() { return base::owner->data__[base::self]; }
};

template <typename T>
struct move_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  move_iterator() : base{} {};

  move_iterator(base b) : base{b} {}

  move_iterator(base::container_t::hierarchy::index_t self,
                base::owner_t *owner)
      : base{self, owner} {}

  base::rvalue_t operator*() {
    return std::move(this->owner->data__[this->self]);
  }
};

template <typename T>
struct copy_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  copy_iterator() : base{} {};

  copy_iterator(base b) : base{b} {}

  copy_iterator(base::container_t::hierarchy::index_t self,
                base::owner_t *owner)
      : base{self, owner} {}

  const base::lvalue_t operator*() {
    return std::move(this->owner->data__[this->self]);
  }
};

template <typename T>
struct root_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  root_iterator() : base{} {};

  root_iterator(base b) : base{b} {}

  root_iterator(base::container_t::hierarchy::index_t self,
                base::owner_t *owner)
      : base{self, owner} {}

  root_iterator &operator++() {
    if (this->valid()) {
      auto &hnode = this->owner->hierarchy__[base::self];

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

template <typename T>
struct sibling_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

  sibling_iterator() : base{} {};

  sibling_iterator(base b) : base{b} {}

  sibling_iterator(base::container_t::hierarchy::index_t self,
                   base::owner_t *owner)
      : base{self, owner} {}

  sibling_iterator &operator++() {
    if (this->valid()) {
      auto next = this->self + 1;
      auto &hierarchy = this->owner->hierarchy__;
      if (hierarchy.size() > next &&
          hierarchy[this->self].parent == hierarchy[next].parent) {
        ++this->self;
      } else {
        this->self = base::container_t::hierarchy::node_t::npos;
      };
    }
    return *this;
  }

  sibling_iterator &operator--() {
    if (this->valid()) {
      auto &hierarchy = this->owner->hierarchy__;
      if (this->self != 0 &&
          hierarchy[this->self].parent == hierarchy[this->self - 1]) {
        --this->self;
      } else {
        this->self = base::container_t::hierarchy::node_t::npos;
      };
    }
    return *this;
  }

  sibling_iterator operator++(int) {
    auto res = this;
    ++*this;
    return res;
  }

  sibling_iterator operator--(int) {
    auto res = this;
    --*this;
    return res;
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

template <typename T> auto childs_of(base_iterator<flat_bfs_type<T>> it) {
  struct : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;
    sibling_iterator<flat_bfs_type<T>> find() {

      if (this->valid()) {
        return base{this->owner->hierarchy__[this->self].ch_begin, this->owner};
      } else if (this->self == base::container_t::hierarchy::node_t::root) {
        return base{0, this->owner};
      } else {
        return base{base::container_t::hierarchy::node_t::npos, this->owner};
      }
    };
  } child_search{it};
  return child_search.find();
}
template <typename T> auto parent_of(base_iterator<flat_bfs_type<T>> it) {
  struct : decltype(it) {
    using base = decltype(it);

    base find() {
      if (this->valid()) {
        return base{this->owner->hierarchy__[this->self].parent, this->owner};
      } else {
        return {};
      };
    };
  } parent_search{it};

  return parent_search.find();
}

template <typename T, typename U>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> from,
                                      base_iterator<flat_bfs_type<U>> to) {
  // UNSAFE
  // NEED TO CHECK HIERARCHY
  struct : decltype(from), decltype(to) {
    decltype(from) value() {
      return decltype(from){decltype(to)::self, decltype(from)::owner};
    };
  } shift{from, to};

  return shift.value();
};

template <typename T>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> lhs,
                                      base_iterator<flat_bfs_type<T>> rhs) {
  return rhs;
};

template <typename T>
base_iterator<flat_bfs_type<T>> shift(base_iterator<flat_bfs_type<T>> from,
                                      base_iterator<hierarchy::bfs> to) {
  struct : decltype(from), decltype(to) {
    decltype(from) value() {
      return decltype(from){decltype(to)::self, decltype(from)::owner};
    };
  } shift{from, to};

  return shift.value();
};

template <typename T> struct reverse_bfs_range_for<flat_bfs_type<T>> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return --this->self, *this; };

    T &operator*() { return this->owner->data__[this->self]; };
  };

  reverse_bfs_range_for(const flat_bfs_type<T> &container)
      : begin_{container.end()} {}

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

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    T &operator*() { return base::owner->data__[base::self]; };
  };

  bfs_range_for(const flat_bfs_type<T> &container)
      : begin_{container.begin()} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T, template <typename> typename Iterator>
struct bfs_iterator_range_for<flat_bfs_type<T>, Iterator> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;
    using iterator_t = Iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    iterator_t operator*() { return base{*this}; };
  };

  bfs_iterator_range_for(const flat_bfs_type<T> &container)
      : begin_{container.begin()} {}

  bfs_iterator_range_for(const flat_bfs_type<T> &container,
                         iterator_type<Iterator>)
      : begin_{container.begin()} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T>
bfs_range_for(const flat_bfs_type<T> &) -> bfs_range_for<flat_bfs_type<T>>;

template <typename T>
bfs_iterator_range_for(const flat_bfs_type<T> &)
    -> bfs_iterator_range_for<flat_bfs_type<T>>;

template <typename T, template <typename> typename Iterator>
bfs_iterator_range_for(const flat_bfs_type<T> &, iterator_type<Iterator>)
    -> bfs_iterator_range_for<flat_bfs_type<T>, Iterator>;

template <typename T>
reverse_bfs_range_for(const flat_bfs_type<T> &)
    -> reverse_bfs_range_for<flat_bfs_type<T>>;

} // namespace iuic::utils::tree
