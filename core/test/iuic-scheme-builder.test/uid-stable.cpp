// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.core;
import iuic.test;
import iuic.underlying;
import iuic.env;
import :scheme.builder;

using namespace iuic;

#include "dummy-layout.inc"

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"uid::stable"};

  void body(iuic::test::utils utils) {
    advance::pool p;
    environment::persist penv{p};
    environment::tmp tenv{p};
    environment::domain denv{};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, denv, {tree.root()}};

    units::uid uid{547665327}; // rand
    style::sid sid{547665327}; // rand

    utils::anchor a;

    auto uid_1 = builder.uid.make_stable("stable", a);
    auto uid_2 = builder.uid.make_stable("stable", a);
    auto uid_3 = builder.uid.make_stable("stable", {});
    auto uid_4 = builder.uid.make_stable("unstable", a); // rand archor

    utils.eq(uid_1, uid_2, "stable uid [a]:[a](no inner)");
    utils.neq(uid_1, uid_3, "stable uid [a]:[rand](no inner)");
    utils.neq(uid_1, uid_4, "stable uid [a]:[a(unstable)](no inner)");

    static dummy_layout layout;

    builder.element.frame(uid, sid, layout, [&](auto &b) {
      auto uid_5 = builder.uid.make_stable("stable", a);
      auto uid_6 = builder.uid.make_stable("stable", a);
      auto uid_7 = builder.uid.make_stable("stable", {});
      utils.neq(uid_1, uid_5, "stable uid [a]:[a](no inner | inner)");
      utils.eq(uid_5, uid_6, "stable uid [a]:[a](inner)");
      utils.neq(uid_3, uid_7, "stable uid [a]:[rand](no inner | inner)");
      utils.neq(uid_6, uid_7, "stable uid [a]:[rand](inner)");
    });
  };
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
