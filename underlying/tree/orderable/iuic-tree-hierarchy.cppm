// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree.orderable:hierarchy;
import std;
import iuic.underlying.iterator;

namespace iuic::tree {
struct basic_ex {};
struct iterator_ex {};
}; // namespace iuic::tree

namespace iuic::tree::hierarchy {

template <typename index_t>
  requires(std::is_integral_v<index_t> && std::is_unsigned_v<index_t>)
struct indexed {
  // operator--() => wrap to root
  static constexpr index_t root = std::numeric_limits<index_t>::max();
  static constexpr index_t npos = root - 1;
  indexed(index_t index_) : index{index_} {};

protected:
  index_t index;
};

template <typename index_t> struct basic_orderable {
  struct node {
    // operator--() => wrap to root
    static constexpr index_t root = std::numeric_limits<index_t>::max();
    static constexpr index_t npos = root - 1;
    index_t parent{root};
    index_t left{npos};
    index_t right{npos};
    index_t ch_first{npos};
    index_t ch_last{npos};
    index_t data{npos};
  };

  std::vector<node> hierarchy;
  node root_{.left = node::root, .right = node::root};
};

export template <typename index_t>
struct orderable : protected basic_orderable<index_t> {

  iuic::iterator::basic<orderable> begin() {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        this, 0};
  };

  iuic::iterator::basic<orderable> end() {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        this, this->hierarchy.size()};
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
        const_cast<orderable *>(this), this->hierarchy.size()};
  };

  iuic::iterator::basic<orderable> root() const {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        const_cast<orderable *>(this), tree::hierarchy::indexed<index_t>::root};
  };

  iuic::iterator::basic<orderable> at(index_t index) const {
    return iuic::iterator::underlying<orderable, iuic::iterator::mode::liner_t>{
        const_cast<orderable *>(this), index};
  };

  std::size_t size() const { return this->hierarchy.size(); };
};
}; // namespace iuic::tree::hierarchy

namespace iuic::iterator {
template <typename index_t>
struct common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>
    : iuic::tree::hierarchy::indexed<index_t> {

  common(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
         index_t index_)
      : iuic::tree::hierarchy::indexed<index_t>{index_}, container{container_} {
  }

  void assign(const iuic::tree::hierarchy::indexed<index_t> &index) {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() { return this->index; };
    } _{index};
    this->index = _.value();
  };

protected:
  iuic::tree::hierarchy::basic_orderable<index_t> *container;
};

template <typename index_t>
struct common<iuic::tree::hierarchy::orderable<index_t>>
    : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> {
  using container_type = iuic::tree::hierarchy::orderable<index_t>;
  using value_type = index_t;
  using unwrap_type = index_t;
  using const_unwrap_type = index_t;

  common(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
         index_t index_)
      : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>{
            container_, index_} {}

  common(
      common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> b)
      : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>{
            b} {}

  common()
      : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>{
            nullptr, 0} {}

  unwrap_type unwrap_as_mutable() { return this->index; };

  const_unwrap_type unwrap_as_const() { return this->index; };
};

template <typename index_t, template <typename...> typename Iterator>
struct common<iuic::tree::hierarchy::orderable<index_t>,
              iterator::extension::iterator_t<Iterator>>
    : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> {
  using container_type = iuic::tree::hierarchy::orderable<index_t>;
  using value_type = index_t;

  common(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
         index_t index_)
      : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>{
            container_, index_} {}

  common(
      common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> b)
      : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>{
            b} {}

  common()
      : common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex>{
            nullptr, 0} {}

  auto unwrap_as_mutable(this auto &self) {
    return Iterator{iuic::iterator::basic{underlying{self}}.drop_ex()};
  };

  auto unwrap_as_const(this const auto &self) {
    return Iterator{iuic::iterator::basic{underlying{self}.drop_ex()}};
  };
};

template <typename index_t, typename... Ex>
struct underlying<iuic::tree::hierarchy::orderable<index_t>,
                  iuic::iterator::mode::liner_t, Ex...>
    : common<iuic::tree::hierarchy::orderable<index_t>, Ex...> {
  struct sentinel {
    sentinel(const underlying &) {};
  };

  underlying(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
             index_t index_)
      : common<iuic::tree::hierarchy::orderable<index_t>>{container_, index_} {}

  underlying(
      common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::hierarchy::orderable<index_t>, Ex...>{c} {}
  underlying() {}

  void increment() { ++this->index; };

  void decrement() { --this->index; };

  bool equal(const sentinel &) const {
    return this->index >= this->container->hierarchy.size();
  };

  bool equal(const iuic::tree::hierarchy::indexed<index_t> &other) const {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() const { return this->index; };
    } _{other};

    return this->index == _.value();
  };
};

template <typename index_t, typename... Ex>
struct underlying<iuic::tree::hierarchy::orderable<index_t>,
                  iuic::iterator::mode::sibling_t, Ex...>
    : common<iuic::tree::hierarchy::orderable<index_t>, Ex...> {

  struct sentinel {
    sentinel(const underlying &) {};
  };
  underlying(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
             index_t index_)
      : common<iuic::tree::hierarchy::orderable<index_t>>{container_, index_} {}

  underlying(
      common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::hierarchy::orderable<index_t>, Ex...>{c} {}
  underlying() {}

  void increment() {
    this->index = this->container->hierarchy[this->index].right;
  };

  void decrement() {
    this->index = this->container->hierarchy[this->index].left;
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

template <typename index_t, typename... Ex>
struct underlying<iuic::tree::hierarchy::orderable<index_t>,
                  iuic::iterator::mode::root_t, Ex...>
    : common<iuic::tree::hierarchy::orderable<index_t>, Ex...> {
  struct sentinel {
    sentinel(const underlying &) {};
  };

  underlying(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
             index_t index_)
      : common<iuic::tree::hierarchy::orderable<index_t>>{container_, index_} {}

  underlying(
      common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::hierarchy::orderable<index_t>, Ex...>{c} {}
  underlying() {}

  void increment() {
    this->index = this->container->hierarchy[this->index].parent;
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
};

template <typename index_t, typename... Ex>
struct underlying<iuic::tree::hierarchy::orderable<index_t>,
                  iuic::iterator::mode::reverse_t, Ex...>
    : common<iuic::tree::hierarchy::orderable<index_t>, Ex...> {
  struct sentinel {
    sentinel(const underlying &) {};
  };

  underlying(iuic::tree::hierarchy::basic_orderable<index_t> *container_,
             index_t index_)
      : common<iuic::tree::hierarchy::orderable<index_t>>{container_, index_} {}

  underlying(
      common<iuic::tree::hierarchy::orderable<index_t>, iuic::tree::basic_ex> c)
      : common<iuic::tree::hierarchy::orderable<index_t>, Ex...>{c} {}
  underlying() {}

  void increment() { --this->index; };

  void decrement() { ++this->index; };

  bool equal(const sentinel &) const { return this->index == this->root; };

  bool equal(const iuic::tree::hierarchy::indexed<index_t> &other) const {
    struct : iuic::tree::hierarchy::indexed<index_t> {
      index_t value() const { return this->index; };
    } _{other};

    return this->index == _.value();
  };
};
} // namespace iuic::iterator
