// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.core;
import iuic.underlying;
import iuic.env;
import :scheme.builder;

using namespace iuic;

#include "dummy-layout.inc"

extern "C++" int main() {
  advance::pool p;
  environment::persist penv{p};
  environment::tmp tenv{p};

  tree::flat_unordered_type<scheme::sketch::value_t> tree;

  scheme::builder builder{penv, tenv, {tree.root()}};

  auto uid_1 = builder.uid.make_ordered("unique");
  auto uid_2 = builder.uid.make_ordered("unique");

  if (uid_1 != uid_2) {
    return 1;
  }

  static dummy_layout layout;

  style::sid sid{0};
  bool res{false};
  builder.element.frame(uid_1, sid, layout, [&](auto &b) { // <-- root ch
    auto uid_3 = b.uid.make_ordered("unique");
    res = uid_1 == uid_3;
  });

  if (res) {
    return 1;
  }
};
