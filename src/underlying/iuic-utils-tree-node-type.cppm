

export module iuic.underlying:utils.tree.node;
import :utils;
import :utils.tree.decl;

export namespace iuic::utils::tree {

template <erasure::as_pure_type T> struct node_type {
private:
  struct node {
    node *parent{nullptr};
    node *left{nullptr};
    node *right{nullptr};
    node *child{nullptr};
    T *value{nullptr};
    static constexpr node *root_wall_ptr() {
      static node _;
      return &_;
    };
  };

public:
  // ----- ITERATORS ----- //
  using base_iterator = base_iterator<node_type>;
  using access_iterator = access_iterator<node_type>;
  using const_access_iterator = const_access_iterator<node_type>;
  using root_iterator = root_iterator<node_type>;
  using const_root_iterator = const_root_iterator<node_type>;
  using sibling_iterator = sibling_iterator<node_type>;
  using const_sibling_iterator = const_sibling_iterator<node_type>;
  using insert_iterator = insert_iterator<node_type>;
  friend base_iterator;
  friend access_iterator;
  friend const_access_iterator;
  friend root_iterator;
  friend const_root_iterator;
  friend sibling_iterator;
  friend const_sibling_iterator;
  friend insert_iterator;
  // ----- ITERATORS ----- //

  base_iterator begin() { return {&root}; };

private:
  node root{.parent = node::root_wall_ptr(),
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

  bool valid() const { return self; };

  operator bool() const { return valid(); };

  bool operator==(const sentinel<node_type<T>> &) { return valid(); };

  base_iterator() : self{nullptr} {}

  base_iterator(container_t::node *self_) : self{self_} {}

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
struct insert_iterator<node_type<T>> : base_iterator<node_type<T>> {
  using base = base_iterator<node_type<T>>;

  insert_iterator(base b) : base{b} {}

  void operator=(T &&);

  void operator=(const T &);

  insert_iterator at(T &&value) {
    using node = typename base::container_t::node;
    using ptr_t = base::pointer_t;
    using value_t = base::value_t;

    if (base::valid())
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
};

template <typename T>
insert_iterator(base_iterator<node_type<T>>) -> insert_iterator<node_type<T>>;

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
}; // namespace iuic::utils::tree
