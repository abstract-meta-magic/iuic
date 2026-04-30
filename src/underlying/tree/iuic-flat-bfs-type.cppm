// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:flat.bfs;
import :decl;
export import :flat.hierarchy.bfs;

//////////////////////////////////////////////////////////////
/// DECL//////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

export namespace iuic::tree {

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

  template <typename Other>
  explicit flat_bfs_type(tree::copy_iterator<Other> it) {
    using other_base_iterator = decltype(it)::base;

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
          std::println("move : {}", counter);
          return;
        }
      }

      if (not cur[index].valid()) {
        // sep
        if (parent != hierarchy::node_t::root) {
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
      this->data__.push_back(*tree::copy_iterator{cur[index]});
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

  template <typename Other>
  explicit flat_bfs_type(tree::move_iterator<Other> it) {
    using other_base_iterator = decltype(it)::base;

    std::vector<other_base_iterator> cur{};
    std::vector<other_base_iterator> next{};

    for (auto ch : iterator_range_for{childs_of(it),
                                      iterator_type<tree::base_iterator>{}}) {
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
          std::println("move : {}", counter);
          return;
        }
      }

      if (not cur[index].valid()) {
        // sep
        if (parent != hierarchy::node_t::root) {
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
      this->data__.push_back(*tree::move_iterator{cur[index]});
      this->hierarchy__.push_back({});
      this->hierarchy__.back().parent = parent;
      ++counter;

      for (auto ch : tree::iterator_range_for{
               childs_of(cur[index]), iterator_type<tree::base_iterator>{}}) {
        next.push_back(ch);
      }

      if (not deep && not next.empty()) {
        deep = next.back().valid();
      }

      next.push_back({}); // sep
    }
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

public: // operations
  bool valid() const {
    return owner && self < owner->data__.size() && not owner->data__.empty();
  };

  bool is_root() const {
    return owner && self == container_t::hierarchy::node_t::root;
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
  operator base_iterator<hierarchy::bfs>() const { return {self, owner}; }

  operator bool() const { return valid(); }

protected:
  container_t::hierarchy::index_t self{container_t::npos};
  flat_bfs_type_base<T> *owner{nullptr};
};

template <typename T>
struct access_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

public: // access
  base::pointer_t operator->() { return &base::owner->data__[base::self]; }

  base::lvalue_t operator*() { return base::owner->data__[base::self]; }

public: // ctor's
  access_iterator(base::container_t::hierarchy::index_t self,
                  base::owner_t *owner)
      : base{self, owner} {}

  access_iterator() : base{} {};

  explicit access_iterator(base b) : base{b} {};
};

template <typename T>
struct const_access_iterator<flat_bfs_type<T>>
    : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

public: // access
  const T *operator->() { return &base::owner->data__[base::self]; }

  const T &operator*() { return base::owner->data__[base::self]; }

public: // ctor's
  const_access_iterator(base::container_t::hierarchy::index_t self,
                        base::owner_t *owner)
      : base{self, owner} {}

  const_access_iterator() : base{} {};

  explicit const_access_iterator(base b) : base{b} {};
};

template <typename T>
struct move_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

public: // access
  base::rvalue_t operator*() {
    return std::move(this->owner->data__[this->self]);
  }

public: // ctor's
  move_iterator() : base{} {};

  move_iterator(base::container_t::hierarchy::index_t self,
                base::owner_t *owner)
      : base{self, owner} {}

  explicit move_iterator(base b) : base{b} {}
};

template <typename T>
struct copy_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

public: // access
  const base::lvalue_t operator*() {
    return std::move(this->owner->data__[this->self]);
  }

public: // ctor's
  copy_iterator() : base{} {};

  copy_iterator(base::container_t::hierarchy::index_t self,
                base::owner_t *owner)
      : base{self, owner} {}

  explicit copy_iterator(base b) : base{b} {}
};

template <typename T>
struct root_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

public: // operators
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

public: // ctor's
  root_iterator() : base{} {};

  root_iterator(base::container_t::hierarchy::index_t self,
                base::owner_t *owner)
      : base{self, owner} {}

  explicit root_iterator(base b) : base{b} {}
};

template <typename T>
struct sibling_iterator<flat_bfs_type<T>> : base_iterator<flat_bfs_type<T>> {
  using base = base_iterator<flat_bfs_type<T>>;

public: // operators
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

public: // ctor's
  sibling_iterator() : base{} {};

  sibling_iterator(base::container_t::hierarchy::index_t self,
                   base::owner_t *owner)
      : base{self, owner} {}

  explicit sibling_iterator(base b) : base{b} {}
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
        return sibling_iterator<flat_bfs_type<T>>{
            base{this->owner->hierarchy__[this->self].ch_begin, this->owner}};
      } else if (this->self == base::container_t::hierarchy::node_t::root) {
        return sibling_iterator<flat_bfs_type<T>>{base{0, this->owner}};
      } else {
        return sibling_iterator<flat_bfs_type<T>>{
            base{base::container_t::hierarchy::node_t::npos, this->owner}};
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
} // namespace iuic::tree
