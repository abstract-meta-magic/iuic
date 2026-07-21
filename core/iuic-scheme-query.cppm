// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.query;
import std;
import :scheme.base;

namespace iuic::scheme::query {
namespace tag {

export struct element : iuic::query::tag {};

export template <const iuic::event::channel &channel__>
struct event : iuic::query::tag {
  static constexpr auto &channel = channel__;
};
} // namespace tag

export template <const iuic::event::channel &channel__>
constexpr iuic::query::expr<tag::event<channel__>> event{};

export constexpr iuic::query::expr<tag::element> element;

export struct hit {
  using type_tag = iuic::query::type;
  units::ui::position point;
};

export struct uid {
  using type_tag = iuic::query::type;
  units::uid value;
};

export struct has_state {
  using type_tag = iuic::query::type;
  iuic::state::value state;
};

export struct valid_t {
  using type_tag = iuic::query::type;
} valid;

export template <erasure::is_pure_type T> struct type {
  using type_tag = iuic::query::type;
};

export struct state {
  using type_tag = iuic::query::type;
  iuic::state::value value;
};

export template <typename EXPR> struct qnot {
  using type_tag = iuic::query::type;
  EXPR expr;
};
}; // namespace iuic::scheme::query
