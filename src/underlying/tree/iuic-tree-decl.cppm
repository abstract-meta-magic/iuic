// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:decl;
import std;
import iuic.underlying.utils;

export namespace iuic::tree {

// TODO : normal name to enum
enum class order_type { dfs, bfs };

namespace tags {
struct bfs_t {
} constexpr inline bfs{};
struct dfs_t {
} constexpr inline dfs{};
}; // namespace tags

enum class iterator_state { valid, invalid, root };

enum class walk_op {
  to_root,   //
  to_parent, //
  to_childs, //
  r_shift,   // single right shift
  l_shift,   // single left shift
  re_shift,  // shift to right last
  le_shift,  // shift to left last
  end,
};

struct insert_traits {
  enum class bfs_insert_op { ins, sep, deep, end };

  enum class dfs_insert_op { at, to, ret, end };

  template <order_type t> static consteval auto get_enum_type() {
    if constexpr (t == order_type::bfs) {
      return bfs_insert_op{};
    } else {
      return dfs_insert_op{};
    };
  };
};

template <order_type t>
using insert_op = decltype(insert_traits::get_enum_type<t>());

// --
template <erasure::is_pure_type T> struct base_iterator;
// acc
template <erasure::is_pure_type T> struct access_iterator;
template <erasure::is_pure_type T> struct const_access_iterator;
// semantic
template <erasure::is_pure_type T> struct move_iterator;
template <erasure::is_pure_type T> struct copy_iterator;
// move
template <erasure::is_pure_type T> struct root_iterator;
template <erasure::is_pure_type T> struct sibling_iterator;
// ins
template <erasure::is_pure_type T> struct insert_iterator;

// --
template <erasure::is_pure_type T> struct sentinel {
  sentinel(const T &) {}
  sentinel(const base_iterator<T>) {}
  sentinel() {};
};

// --

template <template <typename> typename T> struct iterator_type {
  static_assert(false, "Invalid iterator type");
};

template <> struct iterator_type<base_iterator> {};
template <> struct iterator_type<access_iterator> {};
template <> struct iterator_type<const_access_iterator> {};
template <> struct iterator_type<root_iterator> {};
template <> struct iterator_type<sibling_iterator> {};

// --
template <erasure::is_pure_type T,
          template <typename> typename Iterator = base_iterator>
  requires std::is_base_of_v<base_iterator<T>, Iterator<T>>
struct iterator_range_for;
template <erasure::is_pure_type T> struct dfs_range_for;
template <erasure::is_pure_type T> struct iterator_dfs_range_for;
template <erasure::is_pure_type T> struct bfs_range_for;

template <erasure::is_pure_type T,
          template <typename> typename Iterator = base_iterator>
  requires std::is_base_of_v<base_iterator<T>, Iterator<T>>
struct bfs_iterator_range_for;

template <erasure::is_pure_type T> struct reverse_bfs_range_for;

struct invalide_iterator_type {};

template <typename T> struct traits {};

template <typename T>
concept is_base_iterator =
    std::same_as<T, typename traits<std::remove_cvref_t<T>>::base_iterator_t> &&
    requires {
      typename T::container_t;
      typename T::value_t;
      typename T::lvalue_t;
      typename T::rvalue_t;
      typename T::pointer_t;
    };

template <typename T>
concept is_access_iterator = requires(T iterator) {
  {
    typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
  } -> is_base_iterator;
};

template <typename T>
concept is_sibling_iterator = requires(T iterator) {
  {
    typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
  } -> is_base_iterator;
  {
    typename traits<std::remove_cvref_t<T>>::access_iterator_t{iterator}
  } -> is_access_iterator;
};

template <typename T>
concept is_root_iterator = requires(T iterator) {
  {
    typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
  } -> is_base_iterator;
  {
    typename traits<std::remove_cvref_t<T>>::access_iterator_t{iterator}
  } -> is_access_iterator;
};

template <typename T>
concept is_insert_iterator =
    requires(T iterator, typename T::lvalue_t l, typename T::rvalue_t r) {
      {
        typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
      } -> is_base_iterator;
      {
        typename traits<std::remove_cvref_t<T>>::access_iterator_t{iterator}
      } -> is_access_iterator;
      iterator = l;
      iterator = std::move(r);
    };

template <typename T>
consteval access_iterator<T> access_iterator_from(sibling_iterator<T>);

template <typename T> struct traits<base_iterator<T>> {
  using base_iterator_t = base_iterator<T>;
  using container_t = T;
};

template <typename T> struct traits<access_iterator<T>> {
  using base_iterator_t = base_iterator<T>;
};

template <typename T> struct traits<sibling_iterator<T>> {
  using base_iterator_t = base_iterator<T>;
  using access_iterator_t = access_iterator<T>;
};

// end traits

template <typename T>
auto childs_of(base_iterator<T> it)
  requires is_base_iterator<decltype(it)>;

template <typename T>
auto siblings_of(base_iterator<T> it)
  requires is_base_iterator<decltype(it)>;

template <typename T>
auto parent_of(base_iterator<T> it)
  requires is_base_iterator<decltype(it)>;

template <typename T>
concept has_tree_walk = requires(T iterator) {
  { root_of(iterator) } -> is_root_iterator;
  { childs_of(iterator) } -> is_sibling_iterator;
  { siblings_of(iterator) } -> is_sibling_iterator;
};

template <typename T> base_iterator<T> begin(T &&);

template <typename T> sentinel<base_iterator<T>> end(T &&) { return {}; };

template <typename T, typename U>
base_iterator<T> shift(base_iterator<T> from, base_iterator<U> to);

template <typename T, template <typename> typename Iterator>
struct iterator_range_for<root_iterator<T>, Iterator> {
  struct iterator : root_iterator<T> {
    using base = base_iterator<T>;
    using target = Iterator<T>;

    target operator*() { return target{base{*this}}; };
  };

  iterator begin() { return begin_; };

  sentinel<typename iterator::base> end() { return {}; };

  iterator_range_for(root_iterator<T> it, iterator_type<Iterator> = {})
      : begin_{it} {}

private:
  iterator begin_;
};

template <typename T, template <typename> typename Iterator>
struct iterator_range_for<sibling_iterator<T>, Iterator> {
  struct iterator : sibling_iterator<T> {
    using base = base_iterator<T>;
    using target = Iterator<T>;

    target operator*() { return target{base{*this}}; };
  };

  iterator begin() { return begin_; };

  sentinel<typename iterator::target> end() { return {}; };

  iterator_range_for(sibling_iterator<T> it, iterator_type<Iterator> = {})
      : begin_{it} {}

private:
  iterator begin_;
};

template <typename T, template <typename> typename Iterator = base_iterator>
iterator_range_for(root_iterator<T>, iterator_type<Iterator> = {})
    -> iterator_range_for<root_iterator<T>, Iterator>;

template <typename T, template <typename> typename Iterator = base_iterator>
iterator_range_for(sibling_iterator<T>, iterator_type<Iterator> = {})
    -> iterator_range_for<sibling_iterator<T>, Iterator>;

}; // namespace iuic::tree
