// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:dispatcher;
import iuic.underlying;
import :decl;

export namespace iuic::event {
template <const channel &ch> struct dispatcher {
  static void trigger(package pkg, auto &&arg) {
    pkg.call(erasure::visited::as_mutable{arg});
  };
};
}; // namespace iuic::event
