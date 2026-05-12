// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:impl.ranges.type.bfs;
import :decl;
import :flat.bfs;

namespace iuic::tree {

template <typename T> struct reverse_bfs_range_trait {
  using container_t = flat_bfs_type<T>;
  using value_t = T;
  using tag_t = tag::reverse_levelorder;
};
}; // namespace iuic::tree

export namespace iuic::tree {

template <typename T> struct container_range_trait<flat_bfs_type<T>> {
  using container_t = flat_bfs_type<T>;
  using value_t = T;
  using tag_t = tag::bfs;
};

template <typename T>
struct range_for<container_range_trait<flat_bfs_type<T>>> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    T &operator*() { return base::owner->data__[base::self]; };
  };

  explicit range_for(const flat_bfs_type<T> &container)
      : begin_{container.begin()} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T>
range_for(const flat_bfs_type<T> &)
    -> range_for<container_range_trait<flat_bfs_type<T>>>;

template <typename T>
struct range_for<reverse_bfs_range_trait<flat_bfs_type<T>>> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return --base::self, *this; };

    T &operator*() { return base::owner->data__[base::self]; };
  };

  range_for(const flat_bfs_type<T> &container, tag::reverse_levelorder)
      : begin_{container.end()} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T>
range_for(const flat_bfs_type<T> &, tag::reverse_levelorder)
    -> range_for<reverse_bfs_range_trait<flat_bfs_type<T>>>;

template <typename T, template <typename> typename Iterator>
struct iterator_range_for<container_range_trait<flat_bfs_type<T>>, Iterator> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;
    using iterator_t = Iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    iterator_t operator*() { return base{*this}; };
  };

  iterator_range_for(const flat_bfs_type<T> &container, iterator_type<Iterator>)
      : begin_{container.begin()} {}

  iterator_range_for(base_iterator<flat_bfs_type<T>> it,
                     iterator_type<Iterator>)
      : begin_{it} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T, template <typename> typename Iterator>
iterator_range_for(const flat_bfs_type<T> &, iterator_type<Iterator>)
    -> iterator_range_for<container_range_trait<flat_bfs_type<T>>, Iterator>;

template <typename T, template <typename> typename Iterator>
iterator_range_for(base_iterator<flat_bfs_type<T>>, iterator_type<Iterator>)
    -> iterator_range_for<container_range_trait<flat_bfs_type<T>>, Iterator>;

template <typename T, template <typename> typename Iterator>
struct iterator_range_for<reverse_bfs_range_trait<flat_bfs_type<T>>, Iterator> {
  struct iterator : base_iterator<flat_bfs_type<T>> {
    using base = base_iterator<flat_bfs_type<T>>;
    using iterator_t = Iterator<flat_bfs_type<T>>;

    iterator(base b) : base{b} {};

    iterator(base::container_t::hierarchy::index_t i,
             typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return --base::self, *this; };

    iterator_t operator*() { return base{*this}; };
  };

  iterator_range_for(const flat_bfs_type<T> &container, iterator_type<Iterator>,
                     tag::reverse_levelorder)
      : begin_{container.end()} {}

  iterator_range_for(base_iterator<flat_bfs_type<T>> it,
                     iterator_type<Iterator>, tag::reverse_levelorder)
      : begin_{it} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <typename T, template <typename> typename Iterator>
iterator_range_for(const flat_bfs_type<T> &, iterator_type<Iterator>,
                   tag::reverse_levelorder)
    -> iterator_range_for<reverse_bfs_range_trait<flat_bfs_type<T>>, Iterator>;

template <typename T, template <typename> typename Iterator>
iterator_range_for(base_iterator<flat_bfs_type<T>>, iterator_type<Iterator>,
                   tag::reverse_levelorder)
    -> iterator_range_for<reverse_bfs_range_trait<flat_bfs_type<T>>, Iterator>;
}; // namespace iuic::tree
