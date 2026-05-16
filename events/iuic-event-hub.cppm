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

export namespace iuic::event {

struct hub : protected advance::interface {
  struct proxy {
    proxy(hub &hub_) : hub__{std::addressof(hub_)} {}
    proxy(hub *hub_) : hub__{hub_} {}

    template <const channel &CH, typename EVENT_TYPE, typename... PKGR_ARGS>
    void emit(EVENT_TYPE &&event, PKGR_ARGS &&...pkg_args) {
      if (hub__) {
        hub__->emit<CH>(std::forward<EVENT_TYPE>(event),
                        std::forward<PKGR_ARGS>(pkg_args)...);
      };
    };

  private:
    hub *hub__;
  };

  template <const channel &CH, typename EVENT_TYPE, typename... PKGR_ARGS>
  void emit(EVENT_TYPE &&event, PKGR_ARGS &&...pkg_args) {
    event::pool<CH> &pool = get_pool__<CH>();
    allocator<CH> &alloc = get_allocator__<CH>();

    packager<CH> pkgr = [&]() constexpr {
      if constexpr (CH.hub_provide.packanger ==
                    policy::hub_provide::status::front) {

        return packager<CH>{*this, std::forward<PKGR_ARGS>(pkg_args)...};
      } else if constexpr (CH.hub_provide.packanger ==
                           policy::hub_provide::status::back) {
        return packager<CH>{std::forward<PKGR_ARGS>(pkg_args)..., *this};
      } else {
        return packager<CH>{std::forward<PKGR_ARGS>(pkg_args)...};
      }
    }();

    if constexpr (CH.hub_provide.factory ==
                  policy::hub_provide::status::front) {
      factory<EVENT_TYPE, CH>::process(*this, std::forward<EVENT_TYPE>(event),
                                       pool, pkgr, alloc);
    } else if constexpr (CH.hub_provide.factory ==
                         policy::hub_provide::status::back) {
      factory<EVENT_TYPE, CH>::process(std::forward<EVENT_TYPE>(event), pool,
                                       pkgr, alloc, *this);
    } else {
      factory<EVENT_TYPE, CH>::process(std::forward<EVENT_TYPE>(event), pool,
                                       pkgr, alloc);
    }
  };

  template <const channel &CH>
  decltype(auto) query(std::invocable<query<CH> &> auto &&call) {
    // find,create,validate,lock,call ?
    event::query<CH> q{get_pool__<CH>()};
    return call(q);
  }; // mb return proxy

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

  std::unordered_map<std::size_t, erasure::visited> allocs__;
  std::unordered_map<std::size_t, erasure::visited> pools__;
  advance::pool allocs_adv__;
  advance::pool pool_adv__;
};
}; // namespace iuic::event
