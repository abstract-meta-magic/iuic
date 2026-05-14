// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:hub;
import std;
import iuic.underlying;
import :decl;
import :pool;
import :allocator;
import :packager;
import :query;
import :factory;

export namespace iuic::event {

struct hub : advance::interface {
  template <channel CH, typename EVENT_TYPE, typename... PKGR_ARGS>
  void emit(EVENT_TYPE &&event, PKGR_ARGS &&...pkg_args) {
    packager<CH> pkgr{std::forward<PKGR_ARGS>(pkg_args)...};

    event::pool<CH> &pool = get_pool__<CH>();
    allocator<CH> &alloc = get_allocator__<CH>(pool);

    factory<EVENT_TYPE, CH>::process(std::forward<EVENT_TYPE>(event), pool,
                                     pkgr, alloc);
  };

  template <channel CH>
  decltype(auto) query(std::invocable<query<CH> &> auto &&call) {
    // find,create,validate,lock,call ?
    event::query<CH> q{get_pool__<CH>()};
    return call(q);
  }; // mb return proxy

  void reset();

private:
  template <channel CH> event::pool<CH> &get_pool__();
  template <channel CH> allocator<CH> &get_allocator__(event::pool<CH> &pool);

  // mem
  // mem_adv_pool
  // pool_adv_pool
};
}; // namespace iuic::event
