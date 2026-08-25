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
    environment::domain denv{};

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

    scheme::builder builder{penv, tenv, denv, {tree.root()}};

    auto sid_1 = builder.style.make(style_1);
    auto sid_2 = builder.style.make(style_2);
    auto sid_3 = builder.style.make(style_1);

    utils.neq(sid_1, sid_2, "[style-1]:[style-2]");
    utils.eq(sid_1, sid_3, "[style-1]:[style-1]");

    auto value_invalid = builder.style.get(style::sid{0}); // invalid sid
    utils.rq_false(value_invalid.valid(), "invalid");

    auto value_1 = builder.style.get(sid_1);
    utils.rq_true(value_1, "validate style");
    auto &shape_1 = value_1.get_shape();

    utils.rq_true(std::visit(
                      [&]<typename type>(const type &obj) {
                        if constexpr (std::same_as<type, units::upixel>) {
                          utils.eq(obj, 20_upx, "wrong value");
                          return true;
                        } else {
                          return false;
                        }
                      },
                      shape_1.size.width),
                  "wrong visit type");

    utils.rq_true(std::visit(
                      [&]<typename type>(const type &obj) {
                        if constexpr (std::same_as<type, units::upixel>) {
                          utils.eq(obj, 20_upx, "wrong value");
                          return true;
                        } else {
                          return false;
                        }
                      },
                      shape_1.size.height),
                  "wrong visit type");

    utils.ex_catch<iuic::exception::try_override_freeze>(
        [&]() { builder.style.override(sid_1, [](style::shape &shape) {}); },
        "by default decl is freeze");

    auto sid_4 = builder.style.fork(sid_1);
    utils.ex_nothrow(
        [&]() {
          builder.style.override(sid_4, [](style::shape &shape) {
            shape.size.width = 80_upx;
            shape.size.height = 80_upx;
          });
        },
        "invalid freeze");

    builder.style.freeze(sid_4);
    utils.ex_catch<iuic::exception::try_override_freeze>(
        [&]() { builder.style.override(sid_4, [](style::shape &shape) {}); },
        "invalid freeze");

    auto value_2 = builder.style.get(sid_4);

    utils.rq_true(value_2.valid(), "invalid style");
    auto &shape_2 = value_2.get_shape();

    utils.rq_true(std::visit(
                      [&]<typename type>(const type &obj) {
                        if constexpr (std::same_as<type, units::upixel>) {
                          utils.eq(obj, 80_upx, "wrong value");
                          return true;
                        } else {
                          return false;
                        }
                      },
                      shape_2.size.width),
                  "wrong visit type");

    utils.rq_true(std::visit(
                      [&]<typename type>(const type &obj) {
                        if constexpr (std::same_as<type, units::upixel>) {
                          utils.eq(obj, 80_upx, "wrong value");
                          return true;
                        } else {
                          return false;
                        }
                      },
                      shape_2.size.height),
                  "wrong visit type");
  };
} _{};
} // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(false); };
