// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:query;
import :decl;
import :pool;
import :dispatcher;

namespace iuic::event {
template <const channel &CH> struct query {
  // struct proxy for query result

  struct result {

    template <erasure::is_pure_type T> result type() {
      std::vector<package> res;
      std::copy_if(pkgs.begin(), pkgs.end(), std::back_inserter(res),
                   [](const package &pkg) {
                     return pkg.type == erasure::type::from<T>();
                   });
      return result{res};
    };

    result meta(auto &&call);

    template <typename... ARGS> void trigger(ARGS... args) {
      for (auto pkg : pkgs) {
        dispatcher<CH>::trigger(pkg, args...);
      }
    };

    // TODO : make proxy range for unique trigger args

    std::vector<package> pkgs;
  };

  query(pool<CH> &pool_) : pool{pool_} {};

  // sort by meta
  template <typename T> result type() {
    auto pkgs = pool.list();
    std::vector<package> res;
    std::copy_if(pkgs.begin(), pkgs.end(), std::back_inserter(res),
                 [](const package &pkg) {
                   return pkg.type == erasure::type::from<T>();
                 });
    return result{res};
  };

  // sort by meta
  result meta(auto &&call) {
    auto pkgs = pool.list();
    std::vector<package> res;

    std::copy_if(
        pkgs.begin(), pkgs.end(), std::back_inserter(res), [&](package &pkg) {
          return pkg.meta.try_visit_opt(std::forward<decltype(call)>(call))
              .value_or(false);
        });

    return result{res};
  };

  // etc
private:
  pool<CH> &pool;
};
}; // namespace iuic::event
