// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:query;
import :decl;
import :pool;

namespace iuic::event {
template <channel CH> struct query {
  // struct proxy for query result

  query(pool<CH> &);

  // sort by meta
  template <typename T> void type();

  // sort by meta
  void meta(auto &&call);

  // etc
};
}; // namespace iuic::event
