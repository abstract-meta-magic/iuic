// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.tree:impl.ranges.hierarchy.dfs;
import :decl;
import :flat.hierarchy.dfs;

export namespace iuic::tree {

template <> struct container_range_trait<hierarchy::dfs> {
  using container_t = hierarchy::dfs;
  using value_t = hierarchy::dfs_base::index_t;
  using tag_t = tag::dfs;
};

template <template <typename> typename Iterator>
struct iterator_range_for<container_range_trait<hierarchy::dfs>, Iterator> {
  struct iterator : base_iterator<hierarchy::dfs> {
    using base = base_iterator<hierarchy::dfs>;
    using iterator_t = Iterator<hierarchy::dfs>;

    iterator(base b) : base{b} {};

    iterator(hierarchy::dfs_base::index_t i, typename base::owner_t *owner)
        : base{i, owner} {}

    iterator &operator++() { return ++base::self, *this; };

    iterator_t operator*() { return base{*this}; };
  };

  iterator_range_for(const hierarchy::dfs &container,
                     iterator_type<Iterator> = {})
      : begin_{container.begin()} {}

  iterator begin() { return begin_; };

  sentinel<iterator> end() { return {}; };

  std::pair<iterator, sentinel<iterator>> range() { return {begin_, {}}; };

private:
  iterator begin_;
};

template <template <typename> typename Iterator>
iterator_range_for(const hierarchy::dfs &, iterator_type<Iterator>)
    -> iterator_range_for<container_range_trait<hierarchy::dfs>, Iterator>;

}; // namespace iuic::tree
