// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:dispatcher;
import iuic.underlying;
import :decl;

namespace iuic::event {
template <const channel &ch> struct dispatcher {
  static void trigger(package pkg, auto &&arg) {
    // do job
  };
};
}; // namespace iuic::event
