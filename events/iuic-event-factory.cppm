// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:factory;
import std;
import iuic.underlying;
import :decl;
import :pool;
import :allocator;
import :packager;

export namespace iuic::event {

template <typename EVENT_TYPE, channel ch> struct factory {
  static void process(EVENT_TYPE &&event, pool<ch> &pool, packager<ch> &pkgr,
                      allocator<ch> &alloc) {
    pool.push(pkgr.make(std::forward<EVENT_TYPE>(event), alloc));
  };
};
}; // namespace iuic::event
