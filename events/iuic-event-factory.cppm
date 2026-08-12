// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:factory;
import std;
import iuic.underlying;
import :decl;
import :pool;
import :allocator;
import :packager;

export namespace iuic::event {

template <const channel &ch> struct factory {
  static void process(auto &&event, auto &&...args, pool<ch> &pool,
                      packager<ch> &pkgr, allocator<ch> &alloc) {
    pool.push(pkgr.make(std::forward<decltype(event)>(event), alloc));
  };
};
}; // namespace iuic::event
