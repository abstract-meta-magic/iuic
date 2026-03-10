

export module iuic.underlying:utils.tree.node;
import :utils;
import :utils.tree.decl;

namespace iuic::utils::tree {
template <typename T> struct tree_node {
  tree_node *parent{nullptr};
  tree_node *left{nullptr};
  tree_node *right{nullptr};
  tree_node *child{nullptr};
  T *value{nullptr};
  static constexpr tree_node *root_wall_ptr() {
    static tree_node _;
    return &_;
  };
};
}; // namespace iuic::utils::tree

export namespace iuic::utils::tree {

template <erasure::as_pure_type T> struct node_type {
private:
  using node = tree_node<T>;

public:
  // ----- ITERATORS ----- //
  using base_iterator = base_iterator<node_type>;
  using access_iterator = access_iterator<node_type>;
  using const_access_iterator = const_access_iterator<node_type>;
  using root_iterator = root_iterator<node_type>;
  using const_root_iterator = const_root_iterator<node_type>;
  using sibling_iterator = sibling_iterator<node_type>;
  using const_sibling_iterator = const_sibling_iterator<node_type>;
  using insert_iterator = insert_iterator<type::dfs, node_type>;
  friend base_iterator;
  friend access_iterator;
  friend const_access_iterator;
  friend root_iterator;
  friend const_root_iterator;
  friend sibling_iterator;
  friend const_sibling_iterator;
  friend insert_iterator;
  // ----- ITERATORS ----- //

  // can be nullptr
  base_iterator begin() { return {root_.child}; };

  base_iterator root() { return {&root_}; };

  // delete in other thread
  ~node_type() {
    //
    std::vector<node *> cur;
    std::vector<node *> next;

    for (auto *ch = root_.child; ch;) {
      cur.push_back(ch);
      ch = ch->right;
    }

    for (;;) {
      for (node *c : cur) {

        for (node *ch = c->child; ch;) {
          next.push_back(ch);
          ch = ch->right;
        }

        delete c->value;
        delete c;
      }

      std::swap(cur, next);

      if (cur.empty()) {
        return;
      }

      next.clear();
    }
  };

private:
  node root_{.parent = node::root_wall_ptr(),
             .left = node::root_wall_ptr(),
             .right = node::root_wall_ptr(),
             .child = nullptr};
};

// --------------- ITERATORS ---------------

template <typename T> struct base_iterator<node_type<T>> {
  using container_t = node_type<T>;
  using value_t = std::remove_cvref_t<T>;
  using lvalue_t = T &;
  using rvalue_t = T &&;
  using pointer_t = T *;

  iterator_state inspect() const {
    if (self) {
      return self->value ? iterator_state::valid
             : self->parent == container_t::node::root_wall_ptr()
                 ? iterator_state::root
                 : iterator_state::invalid;
    };
    return iterator_state::invalid;
  };

  bool valid() const { return inspect() == iterator_state::valid; };

  operator bool() const { return valid(); };

  bool operator==(const sentinel<node_type<T>> &) {
    return inspect() == iterator_state::valid;
  };

  base_iterator() : self{nullptr} {}

  base_iterator(container_t::node *self_) : self{self_} {}

  template <typename S>
  friend tree::sibling_iterator<node_type<S>>
  childs_of(base_iterator<node_type<S>> it);

protected:
  container_t::node *self;
};

template <typename T>
struct access_iterator<node_type<T>> : public base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

  base::pointer_t operator->() { return base::self->value; };

  base::lvalue_t operator*() { return *base::self->value; };
};

template <typename T>
struct const_access_iterator<node_type<T>>
    : public base_iterator<node_type<T>> {

  using base = base_iterator<node_type<T>>;

  const base::pointer_t operator->() { return base::self->value; };

  const base::lvalue_t operator*() { return *base::self->value; };
};

template <typename T>
struct root_iterator<node_type<T>> : public access_iterator<node_type<T>> {
  using base = access_iterator<node_type<T>>;

  root_iterator &operator++() {
    if (base::valid()) {
      base::self = base::self->parent;
    }

    return *this;
  }

  root_iterator operator++(int) {
    if (base::valide()) {
      auto *ret{base::self};
      base::self = base::self->parent;
      return ret;
    } else {
      return {};
    };
  }
};

template <typename T>
struct const_root_iterator<node_type<T>>
    : public const_access_iterator<node_type<T>> {
  using base = access_iterator<node_type<T>>;

  const_root_iterator &operator++() {
    if (base::valid()) {
      base::self = base::self->parent;
    }

    return *this;
  }

  const_root_iterator operator++(int) {
    if (base::valide()) {
      auto *ret{base::self};
      base::self = base::self->parent;
      return ret;
    } else {
      return {};
    };
  }
};

template <typename T>
struct sibling_iterator<node_type<T>> : access_iterator<node_type<T>> {
  using base = access_iterator<node_type<T>>;

  sibling_iterator(base::base b) : base{b} {}
  sibling_iterator() : base{nullptr} {}

  sibling_iterator &operator++() {
    if (base::valid()) {
      base::self = base::self->right;
    }

    return *this;
  }

  sibling_iterator operator++(int) {
    if (base::valid()) {
      auto *ret = base::self;
      base::self = base::self->right;
      return typename base::base{ret};
    } else {
      return {};
    }
  }

  sibling_iterator &operator--() {
    if (base::valid()) {
      base::self = base::self->left;
    }

    return *this;
  }

  sibling_iterator operator--(int) {
    if (base::valid()) {
      auto *ret = base::self;
      base::self = base::self->left;
      return typename base::base{ret};
    } else {
      return {};
    }
  }
};

template <typename T>
struct const_sibling_iterator<node_type<T>>
    : const_access_iterator<node_type<T>> {
  using base = access_iterator<node_type<T>>;

  const_sibling_iterator &operator++() {
    if (base::valid()) {
      base::self = base::self->right;
    }

    return *this;
  }

  const_sibling_iterator operator++(int) {
    if (base::valid()) {
      auto *ret = base::self;
      base::self = base::self->right;
      return ret;
    } else {
      return {};
    }
  }

  const_sibling_iterator &operator--() {
    if (base::valid()) {
      base::self = base::self->left;
    }

    return *this;
  }

  const_sibling_iterator operator--(int) {
    if (base::valid()) {
      auto *ret = base::self;
      base::self = base::self->left;
      return ret;
    } else {
      return {};
    }
  }
};

template <typename T>
struct insert_iterator<type::dfs, node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

  insert_iterator(base b, tags::dfs_t = {}) : base{b} {}

  void operator=(T &&);

  void operator=(const T &);

  insert_iterator at(T &&value) {
    using node = typename base::container_t::node;
    using ptr_t = base::pointer_t;
    using value_t = base::value_t;

    if (base::inspect() == iterator_state::invalid)
      return base{};

    ptr_t ptr{nullptr};
    node *new_node{nullptr};

    defer _{[&]() noexcept {
      delete ptr;
      delete new_node;
    }};
    // -----------------
    ptr = new value_t{std::forward<decltype(value)>(value)};
    new_node = new node{};
    // -----------------
    _.cancel();
    new_node->value = ptr;

    node *insert_at{base::self->child};

    if (insert_at) {
      for (; insert_at->right;) {
        insert_at = insert_at->right;
      }

      insert_at->right = new_node;
      new_node->left = insert_at;
    } else {
      base::self->child = new_node;
    }

    return base{new_node};
  };

  void at(const T &);

  void to(T &&);
  void to(const T &);

  // before
  // after
};

template <typename T>
struct copy_iterator<type::bfs, node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;
  copy_iterator(base b, tags::bfs_t = {}) : base{b} {}

  void advance(insert_op<type::bfs> &op);

  const T &get();
};

template <typename T>
struct copy_iterator<type::dfs, node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;
  copy_iterator(base b, tags::dfs_t = {}) : base{b} {}

  void advance(insert_op<type::dfs> &op);

  const T &get();
};

template <typename T>
struct move_iterator<type::bfs, node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;
  move_iterator(base b, tags::bfs_t = {}) : base{b} {
    if (base::valid()) {
      for (auto *c = base::self; c;) {
        cur.push_back(c);
        c = c->right;
      }

      for (auto *ch = base::self->child; ch;) {
        next.push_back(ch);
        ch = ch->right;
      }
    }
  }

  void advance(insert_op<type::bfs> &op_) {
    using op = insert_op<type::bfs>;
    switch (op_) {

    case op::ins: {
      if (cur.size() != ++index) {
        // check sep
        auto *r = cur[index - 1]->right;
        if (r == nullptr) {
          op_ = op::sep;
          break;
        }
      } else {
        // check
        cur = std::move(next);
        next.clear();
        index = 0;
        if (cur.empty()) {
          op_ = op::sep;
        } else {
          op_ = op::deep;
        }
        break;
      }
    };
    case op::sep: {
      if (cur.empty()) {
        op_ = op::end;
        return;
      }
    };
    case op::deep: {
    };
    default: {
      auto *n = cur[index];

      int chc{0};
      for (auto *ch = n->child; ch;) {
        next.push_back(ch);
        ch = ch->right;
        ++chc;
      }

      std::println("childs count : {}", chc);
      op_ = op::ins;
    }
    case op::end: {
      break;
    };
    };
  };

  T &&get() { return std::move(*base::self->value); };

private:
  std::size_t index{0};
  std::vector<tree_node<typename base::value_t> *> cur;
  std::vector<tree_node<typename base::value_t> *> next;
};

template <typename T>
struct move_iterator<type::dfs, node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;
  move_iterator(base b, tags::dfs_t = {}) : base{b} {}

  insert_op<type::dfs> op();

  void advance(insert_op<type::dfs> &op_) {
    using op = insert_op<type::dfs>;

    switch (op_) {
    case op::to: {
    }
    case insert_op<type::dfs>::at: {
      if (base::self->child) {
        base::self = base::self->child;
        op_ = op::at;
      } else if (base::self->left) {
        base::self = base::self->left;
        op_ = op::to;
      } else {
        op_ = op::ret;
      }
      break;
    }
    case insert_op<type::dfs>::ret: {
      // at,ret,end
      break;
    }
    case insert_op<type::dfs>::end: {
      break;
    }
    }
  };

  T &&get() { std::move(*base::self->value); };
};

template <typename T>
insert_iterator(base_iterator<node_type<T>>, tags::dfs_t)
    -> insert_iterator<type::dfs, node_type<T>>;

template <typename T>
access_iterator(base_iterator<node_type<T>>) -> access_iterator<node_type<T>>;

template <typename T>
const_access_iterator(base_iterator<node_type<T>>)
    -> const_access_iterator<node_type<T>>;

template <typename T>
root_iterator(base_iterator<node_type<T>>) -> root_iterator<node_type<T>>;

template <typename T>
const_root_iterator(base_iterator<node_type<T>>)
    -> const_root_iterator<node_type<T>>;

template <typename T>
sibling_iterator(base_iterator<node_type<T>>) -> sibling_iterator<node_type<T>>;

template <typename T>
const_sibling_iterator(base_iterator<node_type<T>>)
    -> const_sibling_iterator<node_type<T>>;

template <typename T>
copy_iterator(base_iterator<node_type<T>>, tags::dfs_t)
    -> copy_iterator<type::dfs, node_type<T>>;

template <typename T>
copy_iterator(base_iterator<node_type<T>>, tags::bfs_t)
    -> copy_iterator<type::bfs, node_type<T>>;

template <typename T>
move_iterator(base_iterator<node_type<T>>, tags::dfs_t)
    -> move_iterator<type::dfs, node_type<T>>;

template <typename T>
move_iterator(base_iterator<node_type<T>>, tags::bfs_t)
    -> move_iterator<type::bfs, node_type<T>>;

template <typename T>
sibling_iterator<node_type<T>> childs_of(base_iterator<node_type<T>> it) {
  if (it) {
    return {it.self->child};
  }
  return {nullptr};
};

}; // namespace iuic::utils::tree
