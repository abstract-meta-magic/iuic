

export module iuic.underlying:utils.tree.node;
import :utils;
import :utils.tree.decl;

// flat_unordered_type;
namespace iuic::utils::tree {
struct unordered_hierarchy_node {
  using index_t = std::uint16_t;
  static constexpr index_t npos = std::numeric_limits<index_t>::max();
  static constexpr index_t root = npos - 1;
  index_t parent;
  index_t left;
  index_t right;
  index_t ch_first;
  index_t ch_last;
};

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

template <erasure::is_pure_type T> struct node_type {
private:
  using node = tree_node<T>;

public:
  // ----- ITERATORS ----- //
  using base_iterator = base_iterator<node_type>;
  using access_iterator = access_iterator<node_type>;
  using const_access_iterator = const_access_iterator<node_type>;
  using root_iterator = root_iterator<node_type>;
  using sibling_iterator = sibling_iterator<node_type>;
  using insert_iterator = insert_iterator<node_type>;
  using move_iterator = move_iterator<node_type>;
  friend base_iterator;
  friend root_iterator;
  friend sibling_iterator;
  friend access_iterator;
  friend const_access_iterator;
  friend move_iterator;
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

  bool valid() const { return self ? self->value != nullptr : false; };

  operator bool() const { return valid(); };

  bool operator!=(const sentinel<base_iterator> &) { return valid(); };

  bool operator==(const sentinel<base_iterator> &) { return not valid(); };

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

  const base::value_t *operator->() { return base::self->value; };

  const base::lvalue_t operator*() { return *base::self->value; };
};

template <typename T>
struct move_iterator<node_type<T>> : public base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

  move_iterator(base b) : base{b} {}

  move_iterator() {}
  // TODO : need ptr to rvalue
  base::pointer_t operator->() { return base::self->value; };

  base::rvalue_t operator*() { return std::move(*base::self->value); };
};

template <typename T>
struct root_iterator<node_type<T>> : public base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

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
struct sibling_iterator<node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

  sibling_iterator(base b) : base{b} {}
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
      return base{ret};
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
      return base{ret};
    } else {
      return {};
    }
  }
};

template <typename T>
struct insert_iterator<node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

  insert_iterator(base b, tags::dfs_t = {}) : base{b} {}

  void operator=(T &&);

  void operator=(const T &);

  insert_iterator at(T &&value) {
    using node = typename base::container_t::node;
    using ptr_t = base::pointer_t;
    using value_t = base::value_t;

    if (not base::self) {
      return base{};
    }

    if (not base::self->value &&
        base::self->parent != tree_node<T>::root_wall_ptr()) {
      return base{};
    }

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

    // TO LONG
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

  // use at
  // insert_iterator &operator=(const T &);

  // use at
  // insert_iterator &operator=(T &&);
};

template <typename T>
insert_iterator(base_iterator<node_type<T>>, tags::dfs_t)
    -> insert_iterator<node_type<T>>;

template <typename T>
access_iterator(base_iterator<node_type<T>>) -> access_iterator<node_type<T>>;

template <typename T>
move_iterator(base_iterator<node_type<T>>) -> move_iterator<node_type<T>>;

template <typename T>
const_access_iterator(base_iterator<node_type<T>>)
    -> const_access_iterator<node_type<T>>;

template <typename T>
root_iterator(base_iterator<node_type<T>>) -> root_iterator<node_type<T>>;

template <typename T>
sibling_iterator(base_iterator<node_type<T>>) -> sibling_iterator<node_type<T>>;

template <typename T>
sibling_iterator<node_type<T>> childs_of(base_iterator<node_type<T>> it) {
  if (it.self) {
    return {it.self->child};
  }
  return {nullptr};
};

}; // namespace iuic::utils::tree
