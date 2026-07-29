// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.core;
import iuic.test;
import iuic.underlying;
import iuic.env;
import :scheme.builder;

using namespace iuic;

namespace {

struct test_persist : iuic::test::unit<test_persist> {
  static constexpr std::string_view name{"style::sid"};

  // other info

  void body(iuic::test::utils utils) {
    using namespace iuic::units::literals;
    advance::pool pool;
    environment::persist penv{pool};
    environment::tmp tenv{pool};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    static constexpr auto style_1 = [](style::decl style) {
      style.shape.size.width = 20_upx;
      style.shape.size.height = 20_upx;
      return style;
    }({});

    static constexpr auto style_2 = [](style::decl style) {
      style.shape.size.width = 40_upx;
      style.shape.size.height = 40_upx;
      return style;
    }({});

    scheme::builder builder{penv, tenv, {tree.root()}};

    auto sid_1 = builder.style.make(style_1);
    auto sid_2 = builder.style.make(style_2);
    auto sid_3 = builder.style.make(style_1);

    utils.neq(sid_1, sid_2, "[style-1]:[style-2]");
    utils.eq(sid_1, sid_3, "[style-1]:[style-1]");

    // нужен ли явный фокр
    // что на самом деле делает make
    // как понять что, ты перезаписал стиль
    /*
      builder.style.make(decl);
      builder.style.fork(sid)
      builder.style.override(sid,part);
      builder.style.try_override(sid,part); // noexcept
      builder.style.freeze(sid); // lock override -> need explict fork
     */
    utils.ex_catch<iuic::exception::try_override_freeze>(
        [&]() { builder.style.override(sid_1, [](style::shape &shape) {}); },
        "by default decl is freeze");

    auto sid_4 = builder.style.fork(sid_1);
    utils.ex_nothrow(
        [&]() { builder.style.override(sid_4, [](style::shape &shape) {}); },
        "invalid freeze");
    builder.style.freeze(sid_4);
    utils.ex_catch<iuic::exception::try_override_freeze>(
        [&]() { builder.style.override(sid_4, [](style::shape &shape) {}); },
        "invalid freeze");

    // ex_catch -> try_override_freeze{};

    // etc...

    auto style = builder.style.get(sid_4);
  };
} _{};
} // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(false); };
