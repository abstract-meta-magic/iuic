// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:query;
import :decl;
import :pool;
import :dispatcher;

namespace iuic::event {

template <typename T>
concept is_query = true;

export template <const channel &CH> struct query {
  static_assert(false, "For this channel query is not defined. Pleas watch "
                       "docs [iuic.event.query]");
};
}; // namespace iuic::event
