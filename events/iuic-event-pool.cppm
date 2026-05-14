// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:pool;
import iuic.underlying;
import :decl;

export namespace iuic::event {

template <channel ch> struct pool : advance::interface {
  pool(advance::pool &p) { rebind(p); };
  // default
  pool();

  void push(package p) {};
};

}; // namespace iuic::event
