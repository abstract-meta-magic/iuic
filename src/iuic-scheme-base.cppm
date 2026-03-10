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

struct sketch {
  struct element {
    std::variant<const layout::frame *, const layout::text *> layout;
    units::uid uid;
    style::sid sid;
    units::ui::zorder zorder;
  };
  environment::tmp tenv;
  utils::tree::flat_bfs_type<element> tree;
};

struct blueprint {
  struct element {
    units::ui::area area;
    units::uid uid;
    style::sid sid;
    units::ui::zorder zorder;
    struct {
      enum flag { discarded = 0, text, measure, applied, arrange };

      void set(flag f, bool value_ = true) { value.set(f, value_); };

      bool has(flag f) { return value.test(f); }

    private:
      std::bitset<8> value;
    } meta;
  };

  environment::tmp *tenv{nullptr};     // ??
  environment::persist *penv{nullptr}; // ??
  utils::tree::flat_bfs_type<element> tree;
  using iterator = decltype(tree)::base_iterator;
};
}; // namespace iuic::scheme
