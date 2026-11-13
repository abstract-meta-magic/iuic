// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree.orderable:impl;
import std;
import iuic.underlying.iterator;
import :hierarchy;

namespace iuic::tree {

export namespace order {
struct type {
  constexpr type() : value{this} {}

  constexpr bool operator==(const type &other) const {
    return value == other.value;
  };

  const type *value;
};

struct unordered_t {
  static constexpr type type;
} unordered;
}; // namespace order

template <typename T>
concept is_order = requires {
  { T::type } -> std::convertible_to<order::type>;
};

export template <typename T> struct organizer {
  struct undefined {};
};

export template <typename T, typename index_t>
struct basic_orderable : hierarchy::basic_orderable<index_t> {
  std::vector<T> data;
  order::type order;
};

export template <typename T, typename index_t = std::size_t>
struct orderable : protected basic_orderable<T, index_t> {
  using hierarchy_t = hierarchy::orderable<index_t>;

  iuic::iterator::basic<orderable> begin() {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        this, 0};
  };

  iuic::iterator::basic<orderable> end() {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        this, static_cast<index_t>(this->hierarchy.size())};
  };

  iuic::iterator::basic<orderable> root() {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        this, tree::hierarchy::indexed<index_t>::root};
  };

  iuic::iterator::basic<orderable> at(index_t index) {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        this, index};
  };

  iuic::iterator::basic<orderable> begin() const {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        const_cast<orderable *>(this), 0};
  };

  iuic::iterator::basic<orderable> end() const {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        const_cast<orderable *>(this),
        static_cast<index_t>(this->hierarchy.size())};
  };

  iuic::iterator::basic<orderable> root() const {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        const_cast<orderable *>(this), tree::hierarchy::indexed<index_t>::root};
  };

  iuic::iterator::basic<orderable> at(index_t index) const {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        const_cast<orderable *>(this), index};
  };

  template <is_order Order>
  void organize([[maybe_unused]] Order, bool aling_data = false) {
    if (this->order == Order::type) {
      return;
    };

    if constexpr (requires {
                    organizer<Order>::process(
                        static_cast<hierarchy::basic_orderable<index_t> &>(
                            *this),
                        std::span<const T>{this->data});
                  }) {
      organizer<Order>::process(
          static_cast<hierarchy::basic_orderable<index_t> &>(*this),
          std::span<const T>{this->data});
    } else {
      organizer<Order>::process(
          static_cast<hierarchy::basic_orderable<index_t> &>(*this));
    }

    aling_data ? align_data() : void();

    this->order = Order::type;
  };

  void align_data() {
    // TODO: impl
  };

  order::type state() const { return this->order; };

  void reserve(index_t I) {
    this->hierarchy.reserve(I);
    this->data.reserve(I);
  };

  std::size_t size() const { return this->hierarchy.size(); };

  void clear() {
    this->data.clear();
    this->hierarchy.clear();
    this->root_.ch_first = this->npos;
    this->root_.ch_last = this->npos;
    this->state_ = order::unordered.type;
  };

public: // CTORS
  orderable() {};

  orderable(index_t I) { reserve(I); }
};

}; // namespace iuic::tree

namespace iuic::iterator {
template <typename T, typename index_t>
struct common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>
    : iuic::tree::hierarchy::indexed<index_t> {

  void assign(const iuic::tree::hierarchy::indexed<index_t> &index) {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() { return this->index; };
    } _{index};
    this->index = _.value();
  };

  operator common<iuic::tree::hierarchy::orderable<index_t>,
                  iuic::tree::basic_ex>() const {
    return common<iuic::tree::hierarchy::orderable<index_t>,
                  iuic::tree::basic_ex>{this->container, this->index};
  };

  common(iuic::tree::basic_orderable<T, index_t> *container_, index_t index_)
      : iuic::tree::hierarchy::indexed<index_t>{index_}, container{container_} {
  }

protected:
  iuic::tree::basic_orderable<T, index_t> *container;
};

template <typename T, typename index_t>
struct common<iuic::tree::orderable<T, index_t>>
    : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> {
  using container_type = iuic::tree::orderable<T, index_t>;
  using value_type = T;
  using unwrap_type = T &;
  using const_unwrap_type = const T &;
  using pointer_type = T *;
  using const_pointer_type = const T *;
  static constexpr bool allow_const_drop{true};

  common(iuic::tree::basic_orderable<T, index_t> *container_, index_t index_)
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{
            container_, index_} {}

  common(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> b)
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{b} {}

  common()
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{nullptr,
                                                                        0} {}

  unwrap_type unwrap_as_mutable() {
    return this->container->data[this->container->hierarchy[this->index].data];
  };

  const_unwrap_type unwrap_as_const() {
    return this->container->data[this->container->hierarchy[this->index].data];
  };

  pointer_type getptr_as_mutable() {
    return static_cast<pointer_type>(std::addressof(
        this->container->data[this->container->hierarchy[this->index].data]));
  };

  const_pointer_type getptr_as_const() {
    return std::addressof(
        this->container->data[this->container->hierarchy[this->index].data]);
  };
};

template <typename T, typename index_t>
struct common<iuic::tree::orderable<T, index_t>, extension::debug_t>
    : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> {
  using container_type = iuic::tree::orderable<T, index_t>;
  using value_type = T;
  using unwrap_type = T &;
  using const_unwrap_type = const T &;
  using pointer_type = T *;
  using const_pointer_type = const T *;
  static constexpr bool allow_const_drop{true};

  common(iuic::tree::basic_orderable<T, index_t> *container_, index_t index_)
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{
            container_, index_} {}

  common(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> b)
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{b} {}

  common()
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{nullptr,
                                                                        0} {}

  std::string unwrap_as_mutable() {
    auto &node = this->container->hierarchy[this->index];
    auto str = std::format(
        "iterator : [ parent = {}, ch_first = {}, ch_last = {}, left "
        "= {}, right = {}]",
        node.parent, node.ch_first, node.ch_last, node.left, node.right);

    return str;
  };

  std::string unwrap_as_const() {
    auto &node = this->container->hierarchy[this->index];
    auto str = std::format(
        "iterator : [ parent = {}, ch_first = {}, ch_last = {}, left "
        "= {}, right = {}]",
        node.parent, node.ch_first, node.ch_last, node.left, node.right);

    return str;
  };
};

template <typename T, typename index_t,
          template <typename...> typename Iterator>
struct common<iuic::tree::orderable<T, index_t>,
              iuic::iterator::extension::iterator<Iterator>>
    : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> {
  using container_type = iuic::tree::orderable<T, index_t>;
  using value_type = T;
  using unwrap_type = T &;
  using const_unwrap_type = const T &;
  using pointer_type = T *;
  using const_pointer_type = const T *;
  static constexpr bool allow_const_drop{true};

  common(iuic::tree::basic_orderable<T, index_t> *container_, index_t index_)
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{
            container_, index_} {}

  common(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> b)
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{b} {}

  common()
      : common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex>{nullptr,
                                                                        0} {}

  auto unwrap_as_mutable(this auto &self) {
    return Iterator{iuic::iterator::basic{self}.drop_ex()};
  };

  auto unwrap_as_const(this const auto &self) {
    return Iterator{iuic::iterator::basic{self}.drop_ex()};
  };
};

template <typename T, typename index_t, typename... Ex>
struct underlying<iuic::tree::orderable<T, index_t>,
                  iuic::iterator::mode::liner_t, Ex...>
    : common<iuic::tree::orderable<T, index_t>, Ex...> {

  struct sentinel {
    sentinel(underlying u) {};
  };

  underlying(iuic::tree::basic_orderable<T, index_t> *container_,
             index_t index_)
      : common<iuic::tree::orderable<T, index_t>>{container_, index_} {}

  underlying(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::orderable<T, index_t>, Ex...>{c} {}

  underlying() {}

  // LINER
  void increment() { ++this->index; };

  void decrement() { --this->index; };

  void insert_at(auto &&...value)
    requires std::constructible_from<T, decltype(value)...>
  {
    // TODO: assert
    if (this->index < this->container->hierarchy.size()) {
      insert_at_node(std::forward<decltype(value)>(value)...);
    }
  };

  void insert_in(auto &&...value)
    requires std::constructible_from<T, decltype(value)...>
  {
    // TODO: assert
    if (this->index == this->root) {
      insert_in_root(std::forward<decltype(value)>(value)...);
    } else if (this->index < this->container->hierarchy.size()) {
      insert_in_node(std::forward<decltype(value)>(value)...);
    }
  };

  bool equal(const sentinel &) const {
    return this->index >= this->container->hierarchy.size();
  };

  bool equal(const iuic::tree::hierarchy::indexed<index_t> &other) const {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() const { return this->index; };
    } _{other};

    return this->index == _.value();
  };

private:
  void insert_at_node(auto &&...value) {
    auto &hierarchy = this->container->hierarchy;
    auto &data = this->container->data;

    if (this->container->hierarchy[this->index].parent == this->root) {
      auto &root = this->container->root_;
      auto left_index = root.ch_last;
      auto insert_index = hierarchy.size();

      hierarchy.push_back({.left = left_index});
      data.emplace_back(std::forward<decltype(value)>(value)...);

      hierarchy[root.ch_last].right = insert_index;
      root.ch_last = insert_index;

      hierarchy[insert_index].data = insert_index;
      this->index = insert_index;
      this->container->order = iuic::tree::order::unordered_t::type;
    } else {
      auto parent_index = hierarchy[this->index].parent;

      auto left_index = hierarchy[parent_index].ch_last;
      auto insert_index = hierarchy.size();

      hierarchy.push_back({.parent = parent_index, .left = left_index});
      data.emplace_back(std::forward<decltype(value)>(value)...);

      hierarchy[left_index].right = insert_index;
      hierarchy[parent_index].ch_last = insert_index;

      hierarchy[insert_index].data = insert_index;
      this->index = insert_index;
      this->container->order = iuic::tree::order::unordered_t::type;
    }
  };

  void insert_in_root(auto &&...value) {
    auto &hierarchy = this->container->hierarchy;
    auto &data = this->container->data;
    auto &root = this->container->root_;

    if (root.ch_last == this->npos) {
      hierarchy.push_back({});
      data.emplace_back(std::forward<decltype(value)>(value)...);
      root.ch_first = 0;
      root.ch_last = 0;

      hierarchy[0].data = 0;
      this->index = 0;
      this->container->order = iuic::tree::order::unordered_t::type;
    } else {
      auto insert_index = hierarchy.size();

      hierarchy.push_back({.left = root.ch_last});
      data.emplace_back(std::forward<decltype(value)>(value)...);

      hierarchy[root.ch_last].right = insert_index;
      root.ch_last = insert_index;

      hierarchy[insert_index].data = insert_index;
      this->index = insert_index;
      this->container->order = iuic::tree::order::unordered_t::type;
    }
  };

  void insert_in_node(auto &&...value) {
    auto &hierarchy = this->container->hierarchy;
    auto &data = this->container->data;

    if (hierarchy[this->index].ch_last == this->npos) {
      auto insert_index = hierarchy.size();
      hierarchy.push_back({.parent = this->index});
      data.emplace_back(std::forward<decltype(value)>(value)...);

      hierarchy[this->index].ch_first = insert_index;
      hierarchy[this->index].ch_last = insert_index;

      hierarchy[insert_index].data = insert_index;
      this->index = insert_index;
      this->container->order = iuic::tree::order::unordered_t::type;
    } else {
      auto insert_index = hierarchy.size();
      hierarchy.push_back(
          {.parent = this->index, .left = hierarchy[this->index].ch_last});
      data.emplace_back(std::forward<decltype(value)>(value)...);

      auto left_index = hierarchy[this->index].ch_last;

      hierarchy[left_index].right = insert_index;
      hierarchy[this->index].ch_last = insert_index;

      hierarchy[insert_index].data = insert_index;
      this->index = insert_index;
      this->container->order = iuic::tree::order::unordered_t::type;
    }
  };
};

template <typename T, typename index_t, typename... Ex>
struct underlying<iuic::tree::orderable<T, index_t>,
                  iuic::iterator::mode::sibling_t, Ex...>
    : common<iuic::tree::orderable<T, index_t>, Ex...> {

  struct sentinel {
    sentinel(underlying u) {};
  };

  underlying(iuic::tree::basic_orderable<T, index_t> *container_,
             index_t index_)
      : common<iuic::tree::orderable<T, index_t>>{container_, index_} {}

  underlying(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::orderable<T, index_t>, Ex...>{c} {}

  underlying() {};

  // LINER
  void increment() {
    auto &hnode = this->container->hierarchy[this->index];

    this->index = hnode.right;
  };

  void decrement() {
    auto &hnode = this->container->hierarchy[this->index];

    this->index = hnode.left;
  };

  bool equal(const iuic::tree::hierarchy::indexed<index_t> &other) const {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() const { return this->index; };
    } _{other};

    return this->index == _.value();
  };

  bool equal(const sentinel &) const {
    return this->index >= this->container->hierarchy.size();
  };

  std::size_t difference(const sentinel &);
};

template <typename T, typename index_t, typename... Ex>
struct underlying<iuic::tree::orderable<T, index_t>,
                  iuic::iterator::mode::root_t, Ex...>
    : common<iuic::tree::orderable<T, index_t>, Ex...> {

  struct sentinel {
    sentinel(underlying u) {};
  };

  underlying(iuic::tree::basic_orderable<T, index_t> *container_,
             index_t index_)
      : common<iuic::tree::orderable<T, index_t>>{container_, index_} {}

  underlying(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::orderable<T, index_t>, Ex...>{c} {}

  underlying() {};

  void increment() {
    auto &hnode = this->container->hierarchy[this->index];

    this->index = hnode.parent;
  };

  bool equal(const iuic::tree::hierarchy::indexed<index_t> &other) const {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() const { return this->index; };
    } _{other};

    return this->index == _.value();
  };

  bool equal(const sentinel &) const {
    return this->index >= this->container->hierarchy.size();
  };
};

template <typename T, typename index_t, typename... Ex>
struct underlying<iuic::tree::orderable<T, index_t>,
                  iuic::iterator::mode::reverse_t, Ex...>
    : common<iuic::tree::orderable<T, index_t>, Ex...> {

  struct sentinel {
    sentinel(underlying u) {};
  };

  underlying(iuic::tree::basic_orderable<T, index_t> *container_,
             index_t index_)
      : common<iuic::tree::orderable<T, index_t>>{container_, index_} {}

  underlying(common<iuic::tree::orderable<T, index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::orderable<T, index_t>, Ex...>{c} {}

  underlying() {}

  // LINER
  void increment() { --this->index; };

  void decrement() { ++this->index; };

  bool equal(const iuic::tree::hierarchy::indexed<index_t> &other) const {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() const { return this->index; };
    } _{other};

    return this->index == _.value();
  };

  bool equal(const sentinel &) const { return this->index == this->root; };
};

} // namespace iuic::iterator

export namespace iuic::tree {

template <typename T, typename Mode, typename index_t>
iuic::iterator::basic<iuic::tree::orderable<T, index_t>,
                      iuic::iterator::mode::sibling_t>
childs_of(iuic::iterator::basic<iuic::tree::orderable<T, index_t>, Mode> it) {
  struct hidden
      : iuic::iterator::basic<iuic::tree::orderable<T, index_t>, Mode> {
    using iterator_type =
        iuic::iterator::basic<iuic::tree::orderable<T, index_t>, Mode>;
    hidden(iterator_type it) : iterator_type{it} {
      auto &hierarchy = this->container->hierarchy;

      if (this->index == this->root) {
        this->index = this->container->root_.ch_first;
      } else {
        this->index = hierarchy[this->index].ch_first;
      }
    }
  } result{it};

  return result;
};
}; // namespace iuic::tree
