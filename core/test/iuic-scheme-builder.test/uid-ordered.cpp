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
  static constexpr std::string_view name{"uid::ordered"};

  void body(iuic::test::utils utils) {
    advance::pool p;
    environment::persist penv{p};
    environment::tmp tenv{p};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, {tree.root()}};

    auto uid_1 = builder.uid.make_ordered("unique");
    auto uid_2 = builder.uid.make_ordered("unique");

    utils.eq(uid_1, uid_2, "unique uid [unique]:[unique](no inner case)");

    static dummy_layout layout;

    style::sid sid{0};
    builder.element.frame(uid_1, sid, layout, [&](auto &b) { // <-- root ch
      auto uid_3 = b.uid.make_ordered("unique");
      auto uid_4 = b.uid.make_ordered("unique");
      utils.neq(uid_1, uid_3,
                "unique uid [unique]:[unique](no inner | inner case)");

      utils.eq(uid_3, uid_4, "unique uid [unique]:[unique](inner case)");
    });
  };
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
