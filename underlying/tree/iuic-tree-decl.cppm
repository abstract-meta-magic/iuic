// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:decl;
import std;
import iuic.underlying.utils;
import iuic.underlying.erasure;

namespace iuic::tree::tag {
struct tag_qualifier__ {};
}; // namespace iuic::tree::tag

export namespace iuic::tree {

namespace tag {

template <typename T>
concept is_tag = std::is_base_of_v<tag_qualifier__, T>;

// UB
struct unspecified : tag_qualifier__ {};
struct invalid : tag_qualifier__ {};
struct undefined : tag_qualifier__ {};

// BASE
struct siblings : tag_qualifier__ {};
struct root : tag_qualifier__ {};

//
struct levelorder : tag_qualifier__ {};
struct reverse_levelorder : tag_qualifier__ {};
struct postorder : tag_qualifier__ {};
struct preorder : tag_qualifier__ {};
}; // namespace tag

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

// Ranges

template <typename T>
concept range_trait = erasure::is_pure_type<T> && requires() {
  typename T::container_t;
  typename T::value_t;
  typename T::tag_t;
} && tag::is_tag<typename T::tag_t>;

template <typename T> struct container_range_trait; // for base impl
template <typename T> struct iterator_range_trait;  // for base impl

template <typename Trait, template <typename> typename Iterator>
concept is_valide_iterator_subsitute =
    range_trait<Trait> &&
    requires(base_iterator<typename Trait::container_t> bit) {
      Iterator<typename Trait::container_t>{bit};
    };

template <range_trait Trait,
          template <typename> typename Iterator = base_iterator>
  requires is_valide_iterator_subsitute<Trait, Iterator>
struct iterator_range_for;

template <range_trait Trait> struct range_for;

template <typename T> auto childs_of(base_iterator<T> it);

template <typename T> auto siblings_of(base_iterator<T> it);

template <typename T> auto parent_of(base_iterator<T> it);

template <typename T> base_iterator<T> begin(T &&c) { return c.begin(); };

template <typename T> sentinel<base_iterator<T>> end(T &&) { return {}; };

template <typename T, typename U>
base_iterator<T> shift(base_iterator<T> from, base_iterator<U> to);

template <typename T> struct iterator_range_trait<sibling_iterator<T>> {
  using container_t = typename sibling_iterator<T>::container_t;
  using value_t = typename sibling_iterator<T>::value_t;
  using tag_t = tag::undefined;
};

template <typename T> struct iterator_range_trait<root_iterator<T>> {
  using container_t = typename root_iterator<T>::container_t;
  using value_t = typename root_iterator<T>::value_t;
  using tag_t = tag::undefined;
};

template <typename T, template <typename> typename Iterator>
struct iterator_range_for<iterator_range_trait<sibling_iterator<T>>, Iterator> {
  struct iterator : sibling_iterator<T> {
    iterator(sibling_iterator<T> it) : sibling_iterator<T>{it} {}
    Iterator<T> operator*() { return Iterator<T>{base_iterator{*this}}; }
  };

  iterator_range_for(sibling_iterator<T> it, iterator_type<Iterator> = {})
      : begin_{it} {};

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

private:
  sibling_iterator<T> begin_;
};

template <typename T, template <typename> typename Iterator>
struct iterator_range_for<iterator_range_trait<root_iterator<T>>, Iterator> {
  struct iterator : root_iterator<T> {
    iterator(root_iterator<T> it) : root_iterator<T>{it} {}
    Iterator<T> operator*() { return Iterator<T>{base_iterator{*this}}; }
  };

  iterator_range_for(root_iterator<T> it, iterator_type<Iterator> = {})
      : begin_{it} {};

  iterator_range_for(root_iterator<T> it) : begin_{it} {};

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

private:
  root_iterator<T> begin_;
};

template <typename T, template <typename> typename Iterator>
iterator_range_for(sibling_iterator<T>, iterator_type<Iterator>)
    -> iterator_range_for<iterator_range_trait<sibling_iterator<T>>, Iterator>;

template <typename T>
iterator_range_for(sibling_iterator<T>)
    -> iterator_range_for<iterator_range_trait<sibling_iterator<T>>,
                          base_iterator>;

template <typename T, template <typename> typename Iterator>
iterator_range_for(root_iterator<T>, iterator_type<Iterator>)
    -> iterator_range_for<iterator_range_trait<root_iterator<T>>, Iterator>;
}; // namespace iuic::tree
