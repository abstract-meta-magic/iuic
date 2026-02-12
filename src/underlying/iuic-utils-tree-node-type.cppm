

export module iuic.underlying:utils.tree.node;
import :utils;
import :utils.tree.decl;

export namespace iuic::utils::tree {

template <erasure::as_pure_type T> struct node_type {
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
  struct base_iterator {
    bool valid() const { return self && self->value; };

    operator bool() const noexcept { return valid(); }

    base_iterator(std::nullptr_t) : self{nullptr} {}
    base_iterator(node *self_) : self{self_} {}

  protected:
    node *self;
  };

  struct access_iterator : public base_iterator {
  protected:
    using base_iterator::self;

  public:
    T *operator->() { return self->value; }

    T &operator*() { return *self; }
  };
  struct root_sentinel {};

  struct sibling_sentinel {};

  // iterator to root element
  struct root_iterator : access_iterator {
  protected:
    using base_iterator::self;

  public:
    root_iterator &operator++() {
      if (self) {
        self = self->parent;
      }

      return *this;
    }

    root_iterator operator++(int) {
      if (self) {
        return {self->parent};
      } else {
        return {nullptr};
      }
    }

    bool operator==(root_sentinel) { return *this; }
  };

  struct sibling_iterator : access_iterator {
  protected:
    using base_iterator::self;

  public:
    sibling_iterator &operator++() {
      if (self) {
        self = self->right;
      }
      return *this;
    };

    sibling_iterator operator++(int) {
      if (self) {
        return self->right;
      } else {
        return {nullptr};
      }
    };

    sibling_iterator &operator--() {
      if (self) {
        self = self->left;
      }
      return *this;
    };

    sibling_iterator operator--(int) {
      if (self) {
        return {self->left};
      } else {
        return {nullptr};
      }
    };

    bool operator==(sibling_sentinel s) { return not self; }
  };

  struct insert_point : base_iterator {
  private:
    using base_iterator::self;

  public:
    insert_point emplace_at(auto &&...args) {
      if (not self)
        return {nullptr};

      T *ptr{nullptr};
      node *new_node{nullptr};

      defer _{[&]() noexcept {
        delete ptr;
        delete new_node;
      }};
      // -----------------
      ptr = new T{std::forward<decltype(args)>(args)...};
      new_node = new node{};
      // -----------------
      _.cancel();

      node *insert_at{self->child};

      if (insert_at) {
        for (; insert_at->right;) {
          insert_at = insert_at->right;
        }

        insert_at->right = new_node;
        new_node->left = insert_at;
      } else {
        self->child = new_node;
      }
      return {new_node};
    };

    insert_point emplace_to(auto &&...args) {
      if (not this->valid())
        return {nullptr};

      T *ptr{nullptr};
      node *new_node{nullptr};
      defer _{[&]() noexcept {
        delete ptr;
        delete new_node;
      }};
      // -----------------
      ptr = new T{std::forward<decltype(args)>(args)...};
      new_node = new node{};
      // -----------------
      _.cancel();

      node *insert_at{self};

      for (; insert_at->right;) {
        insert_at = insert_at->right;
      }
      insert_at->right = new_node;
      new_node->left = insert_at;

      return {new_node};
    };

    insert_point push_at(std::convertible_to<T> auto &&value) {
      return emplace_at(std::forward<decltype(value)>(value));
    };

    insert_point push_to(std::convertible_to<T> auto &&value) {
      return emplace_to(std::forward<decltype(value)>(value));
    };

    /* TODO : __ push at self <-*-> self.right
    insert_point push_after(std::convertible_to<T> auto &&value) {
      return nullptr;
    }
    */
  };

  static sibling_iterator childs_of(sibling_iterator it) { return {it.child}; };

  void dfs();

  void bfs();

  insert_point insert_point() { return {&root}; };

private:
  node root{.parent = node::root_wall_ptr(),
            .left = node::root_wall_ptr(),
            .right = node::root_wall_ptr(),
            .child = nullptr};
};
}; // namespace iuic::utils::tree
