// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:hub;
import std;
import iuic.underlying;
import :decl;
import :pool;
import :allocator;
import :packager;
import :query;
import :factory;

namespace iuic::event {

struct store__ {
  template <const channel &CH> allocator<CH> &get_allocator__() {
    auto index = reinterpret_cast<std::size_t>(std::addressof(CH));

    if (auto find = allocs__.find(index); find != allocs__.end()) {
      return erasure::visited::as_mutable{find->second}.unsafe_visit(
          [](event::allocator<CH> &p) -> event::allocator<CH> & { return p; });
    } else {
      auto ins = allocs__.insert(
          {index, erasure::visited{new event::allocator<CH>{allocs_adv__}}});
      return erasure::visited::as_mutable{ins.first->second}.unsafe_visit(
          [](event::allocator<CH> &p) -> event::allocator<CH> & { return p; });
    };
  };

  template <const channel &CH> event::pool<CH> &get_pool__() {

    auto index = reinterpret_cast<std::size_t>(std::addressof(CH));

    if (auto find = pools__.find(index); find != pools__.end()) {
      return erasure::visited::as_mutable{find->second}.unsafe_visit(
          [](event::pool<CH> &p) -> event::pool<CH> & { return p; });
    } else {
      auto ins = pools__.insert(
          {index, erasure::visited{new event::pool<CH>{pool_adv__}}});
      return erasure::visited::as_mutable{ins.first->second}.unsafe_visit(
          [](event::pool<CH> &p) -> event::pool<CH> & { return p; });
    };
  };

  std::unordered_map<std::size_t, erasure::visited> allocs__;
  std::unordered_map<std::size_t, erasure::visited> pools__;
  advance::pool allocs_adv__;
  advance::pool pool_adv__;
};

export struct emitter : virtual protected store__ {

  template <const channel &ch> void emit(auto &&...args) {
    static_assert(is_packager<packager<ch>>,
                  "Invalid packager implimentation. Please watch docs "
                  "[iuic.event.packager]");

    event::pool<ch> &pool = get_pool__<ch>();
    allocator<ch> &alloc = get_allocator__<ch>();

    if constexpr (requires() {
                    packager<ch>::make(alloc, static_cast<emitter &>(*this),
                                       std::forward<decltype(args)>(args)...);
                  }) {
      pool.push(packager<ch>::make(alloc, static_cast<emitter &>(*this),
                                   std::forward<decltype(args)>(args)...));
    } else {
      pool.push(
          packager<ch>::make(alloc, std::forward<decltype(args)>(args)...));
    }
  }
};

export struct accesser : virtual protected store__ {
  template <const channel &CH>
  decltype(auto) query(std::invocable<query<CH> &> auto &&call) {
    static_assert(is_packager<event::query<CH>>,
                  "Invalid query implimentation. Please watch docs "
                  "[iuic.event.query]");
    event::query<CH> q{get_pool__<CH>()};
    return call(q);
  }; // mb return proxy
};

export struct hub : public emitter,
                    public accesser,
                    protected advance::interface {
  void reset() {
    pool_adv__.advance();
    allocs_adv__.advance();
  };

  hub() {};
  hub(advance::pool &p) { rebind(p); };
  // TODO : NEED BIG-V
  hub(const hub &) = delete;            // hard
  hub &operator=(const hub &) = delete; // hard
  hub(hub &&) = delete;                 // mid
  hub &operator=(hub &&) = delete;      // mid
  ~hub() {
    for (auto &[_, obj] : pools__) {
      erasure::visited::as_garbage{obj}.free();
    };

    for (auto &[_, obj] : allocs__) {
      erasure::visited::as_garbage{obj}.free();
    };
  };

private:
  void advance() override { reset(); };
};
}; // namespace iuic::event
