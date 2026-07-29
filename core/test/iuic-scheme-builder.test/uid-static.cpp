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
  static constexpr std::string_view name{"uid::static"};

  void body(iuic::test::utils utils) {
    advance::pool p;
    environment::persist penv{p};
    environment::tmp tenv{p};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, {tree.root()}};

    auto uid_1 = builder.uid.make_static("STR");

    auto uid_2 = builder.uid.make_static("STR");

    utils.eq(uid_1, uid_2, "static uid [STR]:[STR]");

    auto uid_3 = builder.uid.make_static("STR-");

    utils.neq(uid_1, uid_3, "static uid [STR]:[STR-]");

    static dummy_layout layout;

    style::sid sid{0};
    builder.element.frame(sid, layout, [&](auto &b) {
      auto uid_4 = b.uid.make_static("STR");
      utils.eq(uid_1, uid_4, "inner static uid [STR]:[STR]");
    });
  }
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
