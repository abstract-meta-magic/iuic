// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.base;
import iuic.underlying;
import :style;
import :environment.tmp;
import :environment.persist;

// forward
export namespace iuic::layout {
struct frame;
struct text;
}; // namespace iuic::layout

namespace iuic::scheme {

struct sk_element {
  std::variant<const layout::frame *, const layout::text *> layout;
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
};

struct bp_element {
  units::ui::area area;
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
  struct {
    enum flag { discarded = 0, virtualized, text, measure, applied, arrange };

    void set(flag f, bool value_ = true) { value.set(f, value_); };

    bool has(flag f) { return value.test(f); }

  private:
    std::bitset<8> value{};
  } meta;
};

using sketch = utils::tree::flat_bfs_type<sk_element>;

using blueprint = utils::tree::flat_bfs_type<bp_element>;

export namespace iterators {

using base = utils::tree::base_iterator<utils::tree::hierarchy<blueprint>>;
using root = utils::tree::root_iterator<utils::tree::hierarchy<blueprint>>;
using sibling =
    utils::tree::sibling_iterator<utils::tree::hierarchy<blueprint>>;

sibling childs_of(base it) { return utils::tree::childs_of(it); };

}; // namespace iterators

export namespace ranges {
// level_order
// post_order
};

}; // namespace iuic::scheme
