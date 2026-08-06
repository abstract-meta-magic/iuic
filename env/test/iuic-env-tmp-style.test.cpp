// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.env;
import std;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::env::tmp[style]"};
  void body(iuic::test::utils utils) {
    [&]<typename T = iuic::environment::style_tmp_registry>() {
      if constexpr (iuic::cenv::logic("iuic::debug.api").value_or(false)) {
        iuic::advance::pool pool;
        T ss{pool};

        auto &sheet = ss.get_sheet_debug();

        static constexpr iuic::style::decl style_1{};
        static constexpr iuic::style::decl style_2{};

        utils.rq_true(sheet.shape.empty(), "zero init");
        utils.rq_true(sheet.decoration.empty(), "zero init");
        utils.rq_true(sheet.transform.empty(), "zero init");
        utils.rq_true(sheet.advence.empty(), "zero init");

        auto sid_1 = ss.make(style_1);
        auto sid_2 = ss.make(style_2);
        utils.eq(sheet.shape.size(), 2, "check fill");
        utils.eq(sheet.decoration.size(), 2, "check fill");
        utils.eq(sheet.transform.size(), 2, "check fill");
        utils.eq(sheet.advence.size(), 2, "check fill");

        utils.neq(sid_1, sid_2, "check sid");
        auto sid_3 = ss.make(style_1);
        utils.eq(sid_1, sid_3, "check sid");
        utils.eq(sheet.shape.size(), 2, "check fill");
        utils.eq(sheet.decoration.size(), 2, "check fill");
        utils.eq(sheet.transform.size(), 2, "check fill");
        utils.eq(sheet.advence.size(), 2, "check fill");

        utils.rq_false(ss.override(sid_1,
                                   [](iuic::style::shape &shape) {
                                     //
                                   }),
                       "override decl");
        utils.eq(sheet.shape.size(), 2, "check fill");
        utils.eq(sheet.decoration.size(), 2, "check fill");
        utils.eq(sheet.transform.size(), 2, "check fill");
        utils.eq(sheet.advence.size(), 2, "check fill");

        auto sid_4 = ss.fork(sid_1);

        utils.rq_true(ss.override(sid_4,
                                  [](iuic::style::shape &shape) {
                                    //
                                  }),
                      "override fork");
        utils.eq(sheet.shape.size(), 3, "check fill");
        utils.eq(sheet.decoration.size(), 2, "check fill");
        utils.eq(sheet.transform.size(), 2, "check fill");
        utils.eq(sheet.advence.size(), 2, "check fill");

        ss.freeze(sid_4);

        utils.rq_false(ss.override(sid_4,
                                   [](iuic::style::shape &shape) {
                                     //
                                   }),
                       "override frozen");

        utils.eq(sheet.shape.size(), 3, "check fill");
        utils.eq(sheet.decoration.size(), 2, "check fill");
        utils.eq(sheet.transform.size(), 2, "check fill");
        utils.eq(sheet.advence.size(), 2, "check fill");

        pool.advance();

        utils.rq_true(sheet.shape.empty(), "advance drop");
        utils.rq_true(sheet.decoration.empty(), "advance drop");
        utils.rq_true(sheet.transform.empty(), "advance drop");
        utils.rq_true(sheet.advence.empty(), "advance drop");

      } else {
        // make warning
      }
    }();

    //
    // case
  }
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); }
