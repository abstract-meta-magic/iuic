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

  units::uid uid{547665327}; // rand
  style::sid sid{547665327}; // rand

  utils::anchor a;

  auto uid_1 = builder.uid.make_stable("stable", a);
  auto uid_2 = builder.uid.make_stable("stable", a);
  auto uid_3 = builder.uid.make_stable("stable", {});
  auto uid_4 = builder.uid.make_stable("unstable", a); // rand archor

  if (uid_1 != uid_2) {
    std::println("1");
    return 1;
  }

  if (uid_1 == uid_3) {
    return 1;
  }

  if (uid_1 == uid_4) {
    return 1;
  }
  static dummy_layout layout;

  bool res_1;
  bool res_2;
  bool res_3;
  builder.element.frame(uid, sid, layout, [&](auto &b) {
    auto uid_5 = builder.uid.make_stable("stable", a);
    auto uid_6 = builder.uid.make_stable("stable", a);
    auto uid_7 = builder.uid.make_stable("stable", {});
    res_1 = uid_5 == uid_1;
    res_2 = uid_5 != uid_6;
    res_3 = uid_7 == uid_1;
  });

  if (res_1) {
    return 1;
  }

  if (res_2) {
    return 1;
  }

  if (res_3) {
    return 1;
  }

  return 0;
};
