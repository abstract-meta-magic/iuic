// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:impl.ranges.hierarchy.bfs;
import :decl;
import :flat.hierarchy.bfs;

export namespace iuic::tree {

template <> struct container_range_trait<hierarchy::bfs> {
  using container_t = hierarchy::bfs;
  using value_t = hierarchy::bfs_base::index_t;
  using tag_t = tag::bfs;
};

template <template <typename> typename Iterator>
struct iterator_range_for<container_range_trait<hierarchy::bfs>, Iterator> {
  struct iterator : base_iterator<hierarchy::bfs> {
    using base = base_iterator<hierarchy::bfs>;
    using iterator_t = Iterator<hierarchy::bfs>;

    iterator(base b) : base{b} {};

    iterator(hierarchy::bfs_base::index_t i, typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    iterator_t operator*() { return base{*this}; };
  };

  iterator_range_for(const hierarchy::bfs &container,
                     iterator_type<Iterator> = {})
      : begin_{container.begin()} {}

  iterator_range_for(base_iterator<hierarchy::bfs> it,
                     iterator_type<Iterator> = {})
      : begin_{it} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <template <typename> typename Iterator>
iterator_range_for(const hierarchy::bfs &, iterator_type<Iterator>)
    -> iterator_range_for<container_range_trait<hierarchy::bfs>, Iterator>;

template <template <typename> typename Iterator>
iterator_range_for(base_iterator<hierarchy::bfs> it, iterator_type<Iterator>)
    -> iterator_range_for<container_range_trait<hierarchy::bfs>, Iterator>;

}; // namespace iuic::tree
