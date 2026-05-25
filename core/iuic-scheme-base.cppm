// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.base;
import std;
import iuic.underlying;
import iuic.text;
import iuic.layout;
import iuic.style;
import iuic.env;

import iuic.state; // tmp

namespace iuic::scheme {

struct sk_element {
  std::variant<const layout::frame *, const layout::text *> layout;
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
  std::span<const text::raw::token> text;
};

struct bp_element {
  units::ui::area area;
  units::uid uid;
  style::sid sid;
  units::ui::zorder zorder;
  std::span<const text::present::token> text;
  struct {
    enum flag { discarded = 0, virtualized, text, measure, applied, arrange };

    void set(flag f, bool value_ = true) { value.set(f, value_); };

    bool has(flag f) const { return value.test(f); }

  private:
    std::bitset<8> value{};
  } meta;
};

using sketch = tree::flat_bfs_type<sk_element>;

using blueprint = tree::flat_bfs_type<bp_element>;

export namespace iterators {

using base = tree::base_iterator<tree::hierarchy::bfs>;
using sentinel = tree::sentinel<base>;
using root = tree::root_iterator<tree::hierarchy::bfs>;
using sibling = tree::sibling_iterator<tree::hierarchy::bfs>;

sibling childs_of(base it) { return tree::childs_of(it); };

}; // namespace iterators

export namespace ranges {
// level_order

struct level_order {
  struct iterator : iterators::base {};

  iterator begin();

  tree::sentinel<iterator> end();
};

struct reverse_level_order {
  struct iterator : iterators::base {
    using base = iterators::base;
    iterator(base b) : base{b} {}
    iterator() {}

    iterator &operator*() { return *this; }

    iterator &operator++() {
      --self;
      return *this;
    };
  };

  reverse_level_order(iterators::base it) : begin_{find_begin__(it)} {};

  iterator begin() { return begin_; };

  tree::sentinel<iterator> end() { return {}; };

private:
  static iterator find_begin__(iterators::base it) {
    struct : iterators::base {
      iterator find() {
        return iterators::base{this->owner->hierarchy__.size() - 1,
                               this->owner};
      };
    } search_end{it};

    return search_end.find();
  };

private:
  iterator begin_;
};

struct postorder {
  struct iterator : iterators::base {
    using base = iterators::base;
    iterator(base b) : base{b} {}
    iterator() {}

    iterator &operator*() { return *this; }

    iterator &operator++() {
      throw std::runtime_error{"is boroken"};
      return *this;
    };
  };

  postorder(iterators::base it) : begin_{find_entry__(it)} {}

  iterator begin() { return begin_; };

  tree::sentinel<iterator> end() { return {}; };

private:
  static iterator find_entry__(iterators::base it) {
    struct : iterators::base {
      using base = iterator::base;
      iterator find() {
        if (owner && not owner->hierarchy__.empty()) {
          auto &h = owner->hierarchy__;
          index_t sel{0};

          for (; h[sel].ch_begin != npos;) {
            sel = h[sel].ch_begin;
          }
          return base{sel, owner};

        } else {
          return base{};
        }
      }
    } search_entry{it};
    return search_entry.find();
  };

private:
  iterator begin_;
};

struct preorder {
  struct iterator : iterators::base {};
  iterator begin() { throw std::runtime_error{"Not impl"}; };

  tree::sentinel<iterator> end() { return {}; };
};

// post_order
}; // namespace ranges

export namespace query {

struct hit {
  units::ui::position point;
};

struct has_state {
  iuic::state::value state;
};

struct valid_t {
} constexpr valid;

template <typename EXPR> struct qnot {
  EXPR expr;
};

template <typename LEXPR, typename REXPR> struct qor {
  LEXPR lexpr;
  REXPR rexpr;
};

struct branch {
  // later
};
// expr<typename T,typename ...Ts>

template <typename T, typename... Ts> struct expr {

  constexpr expr() {};

  // TODO : make normal copy\move\forward\cat

  template <typename New, typename... OTs>
  constexpr expr(expr<T, OTs...> &&other, New &&v)
      : expr{make_expr(std::move(other), std::forward<New>(v))} {};

  template <typename New, typename... OTs>
  constexpr expr(const expr<T, OTs...> &other, New &&v)
      : expr{make_expr(other, std::forward<New>(v))} {};

  template <std::size_t index> constexpr const auto &at() const {
    return std::get<index>(value);
  };

  template <typename TAG, typename... ARGS>
  constexpr expr(TAG, ARGS &&...args) : value{std::forward<ARGS>(args)...} {}

  static constexpr std::size_t size() { return sizeof...(Ts); };

  static constexpr auto index_sequence() {
    return std::make_index_sequence<size()>{};
  };

private:
  template <typename New, typename... OTs>
  static constexpr auto make_expr(expr<T, OTs...> other, New &&n) {
    if constexpr (sizeof...(OTs) == 0) {
      return expr{T{}, std::forward<New>(n)};
    } else {
      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return expr{T{}, std::get<I>(other.value)..., std::forward<New>(n)};
      }(std::make_index_sequence<sizeof...(OTs)>{});
    }
  };
  template <typename, typename...> friend struct expr;
  std::tuple<Ts...> value;
};

namespace tag {
struct element {};
struct event_local {};
struct event_global {};
}; // namespace tag

template <typename T, typename New, typename... Ts>
expr(expr<T, Ts...> &&, New &&) -> expr<T, Ts..., New>;

template <typename T, typename New, typename... Ts>
expr(const expr<T, Ts...> &, New &&) -> expr<T, Ts..., New>;

template <typename TAG, typename... ARGS>
expr(TAG, ARGS &&...) -> expr<TAG, ARGS...>;

constexpr expr<tag::element> element{};
constexpr expr<tag::event_local> event_local{};
constexpr expr<tag::event_global> event_global{};

template <typename... Ts, typename T, typename New>
constexpr auto operator|(expr<T, Ts...> lhs, New &&rhs) {
  return expr<T, Ts..., New>{std::move(lhs), std::forward<New>(rhs)};
}

constexpr has_state hovered{iuic::state::base::hovered};
}; // namespace query

}; // namespace iuic::scheme
