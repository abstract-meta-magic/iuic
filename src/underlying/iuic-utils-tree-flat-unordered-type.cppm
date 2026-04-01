// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying:utils.tree.unordered;
import std;
import :utils.tree.decl;

namespace iuic::utils::tree {
struct unordered_hierarchy_node_t {
  using index_t = std::size_t; // TODO : replace to template papam
  static constexpr index_t npos = std::numeric_limits<index_t>::max();
  static constexpr index_t root = npos - 1;
  index_t parent{root};
  index_t left{npos};
  index_t right{npos};
  index_t ch_first{npos};
  index_t ch_last{npos};
};

template <typename> struct hierarchy_base;

}; // namespace iuic::utils::tree

export namespace iuic::utils::tree {
template <typename> struct hierarchy;
template <typename> struct flat_unordered_type;

template <typename T> struct hierarchy_base<flat_unordered_type<T>> {
  std::vector<unordered_hierarchy_node_t> hierarchy__;
  unordered_hierarchy_node_t::index_t root_begin{0}, root_end{0};
};

template <typename T>
struct hierarchy<flat_unordered_type<T>>
    : protected hierarchy_base<flat_unordered_type<T>> {};

}; // namespace iuic::utils::tree

export namespace iuic::utils::tree {
template <typename T>
struct flat_unordered_type_base : hierarchy_base<flat_unordered_type<T>> {
  std::vector<T> data__;
};

template <typename T>
struct flat_unordered_type : protected flat_unordered_type_base<T> {
  using base_iterator = base_iterator<flat_unordered_type>;
  using root_iterator = root_iterator<flat_unordered_type>;
  using sibling_iterator = sibling_iterator<flat_unordered_type>;
  using access_iterator = access_iterator<flat_unordered_type>;
  using const_access_iterator = const_access_iterator<flat_unordered_type>;

  base_iterator begin() const {
    return {0, const_cast<flat_unordered_type *>(this)};
  };

  base_iterator end() const {
    // not normal
    return {unordered_hierarchy_node_t::npos,
            const_cast<flat_unordered_type *>(this)};
  };

  base_iterator root() const {
    return {unordered_hierarchy_node_t::root,
            const_cast<flat_unordered_type *>(this)};
  };
};

template <typename T> struct base_iterator<flat_unordered_type<T>> {
protected:
  using owner_t = flat_unordered_type_base<T>;

public:
  using container_t = flat_unordered_type<T>;
  using value_t = T;
  using lvalue_t = T &;
  using rvalue_t = T &&;
  using pointer_t = T *;

  bool valid() const { return owner ? self < owner->data__.size() : false; };

  bool is_root() const {
    return owner ? self == unordered_hierarchy_node_t::root : false;
  };

  template <typename I>
    requires std::is_base_of_v<base_iterator, I>
  bool operator==(const sentinel<I> &) const {
    return not valid();
  };

  template <typename I>
    requires std::is_base_of_v<base_iterator, I>
  bool operator!=(const sentinel<I> &) const {
    return valid();
  };

  operator bool() const { return valid(); };

  base_iterator() {}

  base_iterator(unordered_hierarchy_node_t::index_t self_, owner_t *owner_)
      : self{self_}, owner{owner_} {};

protected:
  unordered_hierarchy_node_t::index_t self{unordered_hierarchy_node_t::npos};
  owner_t *owner{nullptr};
};

template <typename T>
struct root_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;

  root_iterator(base b) : base{b} {}
  root_iterator() {}

  root_iterator &operator++() {
    if (this->valid()) {
      this->self = this->owner->hierarchy__[this->self].parent;
    }
    return *this;
  };

  root_iterator operator++(int) {
    auto res = *this;
    ++*this;
    return res;
  };
};

template <typename T>
struct sibling_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;

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
    --*this;
    return res;
  };
};

template <typename T>
struct access_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;
  access_iterator(base b) : base{b} {}
  access_iterator() {}

  base::lvalue_t operator*() { return this->owner->data__[this->self]; };

  base::pointer_t operator->() {
    return std::addressof(this->owner->data__[this->self]);
  };
};

template <typename T>
struct const_access_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;
  const_access_iterator(base b) : base{b} {}
  const_access_iterator() {}
  const base::lvalue_t operator*() { return this->owner->data__[this->self]; };
  const base::pointer_t operator->() {
    return std::addressof(this->owner->data__[this->self]);
  };
};

template <typename T>
struct copy_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;
  copy_iterator(base b) : base{b} {}
  copy_iterator() {}
  const base::lvalue_t operator*() { return this->owner->data__[this->self]; };
};

template <typename T>
struct move_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;
  move_iterator(base b) : base{b} {}
  move_iterator() {}
  base::rvalue_t operator*() {
    return std::move(this->owner->data__[this->self]);
  };
};

template <typename T>
struct insert_iterator<flat_unordered_type<T>>
    : base_iterator<flat_unordered_type<T>> {
  using base = base_iterator<flat_unordered_type<T>>;

  insert_iterator(base b) : base{b} {};

  insert_iterator() {};

  insert_iterator at(T &&value) {
    if (this->self == unordered_hierarchy_node_t::root && this->owner) {
      return at_root(std::move(value));
    } else if (this->valid()) {
      return at_(std::move(value));
    }
    return {};
  };

  void to(T &&) {};

private:
  insert_iterator at_root(T &&value) {
    auto &hierarchy = this->owner->hierarchy__;
    auto &data = this->owner->data__;
    auto index = hierarchy.size();

    data.push_back(std::move(value));
    if (index == 0) {
      hierarchy.push_back({});
    } else {
      hierarchy.push_back({.left = this->owner->root_end});
      auto &root_end = this->owner->root_end;

      hierarchy[root_end].right = index;
      root_end = index;
    };
    return base{index, this->owner};
  }

  insert_iterator at_(T &&value) {
    auto &hierarchy = this->owner->hierarchy__;
    auto &data = this->owner->data__;
    auto index = hierarchy.size();

    data.push_back(std::move(value));
    auto &self_node = hierarchy[this->self];
    if (self_node.ch_first != unordered_hierarchy_node_t::npos) {
      hierarchy.push_back(
          {.parent = this->self, .left = self_node.ch_last}); // invalidate it
      auto &self_node = hierarchy[this->self];
      hierarchy[self_node.ch_last].right = index;
      self_node.ch_last = index;
    } else {
      hierarchy.push_back({.parent = this->self}); // invalidate it
      auto &self_node = hierarchy[this->self];
      self_node.ch_first = index;
      self_node.ch_last = index;
    }
    return base{index, this->owner};
  };
};

template <typename T>
root_iterator(base_iterator<flat_unordered_type<T>>)
    -> root_iterator<flat_unordered_type<T>>;

template <typename T>
sibling_iterator(base_iterator<flat_unordered_type<T>>)
    -> sibling_iterator<flat_unordered_type<T>>;

template <typename T>
access_iterator(base_iterator<flat_unordered_type<T>>)
    -> access_iterator<flat_unordered_type<T>>;

template <typename T>
const_access_iterator(base_iterator<flat_unordered_type<T>>)
    -> const_access_iterator<flat_unordered_type<T>>;

template <typename T>
copy_iterator(base_iterator<flat_unordered_type<T>>)
    -> copy_iterator<flat_unordered_type<T>>;

template <typename T>
move_iterator(base_iterator<flat_unordered_type<T>>)
    -> move_iterator<flat_unordered_type<T>>;

template <typename T> auto childs_of(base_iterator<flat_unordered_type<T>> it) {
  struct : decltype(it) {
    using base = decltype(it);

    sibling_iterator<typename base::container_t> find() {
      if (this->valid()) {
        return base{this->owner->hierarchy__[this->self].ch_first, this->owner};
      } else if (this->is_root()) {
        return base{0, this->owner};
      } else {
        return {};
      };
    };
  } child_search{it};

  return child_search.find();
};

template <typename T> auto parent_of(base_iterator<flat_unordered_type<T>> it) {
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
};

// TODO : remove this
void heh(base_iterator<flat_unordered_type<int>> it) {
  root_iterator rit{it};
  sibling_iterator sit{it};
  access_iterator ait{it};
  const_access_iterator cait{it};
  copy_iterator cit{it};
};
}; // namespace iuic::utils::tree
