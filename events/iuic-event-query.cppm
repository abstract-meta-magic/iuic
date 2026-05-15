// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:query;
import :decl;
import :pool;
import :dispatcher;

namespace iuic::event {
template <const channel &CH> struct query {
  // struct proxy for query result

  struct result {

    template <typename T> result type();

    result meta(auto &&call);

    template <typename... ARGS> void trigger(ARGS... args) {
      for (auto pkg : pkgs) {
        dispatcher<CH>::trigger(pkg, args...);
      }
    };

    // TODO : make proxy range for unique trigger args

    std::vector<package> pkgs;
  };

  query(pool<CH> &);

  // sort by meta
  template <typename T> result type();

  // sort by meta
  result meta(auto &&call);

  // etc
};
}; // namespace iuic::event
