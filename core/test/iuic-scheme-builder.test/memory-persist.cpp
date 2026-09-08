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

struct test_persist : iuic::test::unit<test_persist> {
  static constexpr std::string_view name{"memory::persist"};

  // other info

  void body(iuic::test::utils utils) {
    advance::pool pool;
    environment::persist penv{pool};
    environment::tmp tenv{pool};
    environment::domain denv{};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, denv, {tree.root()}};

    units::uid uid{547665327}; // rand

    struct data {
      int x, y;
    };
    struct wrong_type {};

    // reserve slot
    auto persist = builder.memory.persist<data>(uid);

    utils.rq_false(persist.get().try_visit([&](data &d) {}),
                   "Visit unintialized value");

    ///----
    bool res_1{false};

    // persist has other type
    persist.init_if_not([]() { return wrong_type{}; });

    // persist has other type
    utils.rq_false(persist.get().try_visit([&](data &d) {}),
                   "Visit unintialized value");
    // persist has other type
    utils.rq_false(persist.get().try_visit([&](wrong_type &d) {}),
                   "Visit wrong type");

    persist.init_if_not([]() { return data{.x = 10, .y = 5}; });

    utils.rq_true(persist.get().try_visit([&](data &d) {
      utils.eq(d.x, 10, "wrong value");
      utils.eq(d.y, 5, "wrong value");
      d.x = 20;
      d.y = 40;
    }),
                  "Object is not visited");

    ///----

    utils.rq_true(persist.get().try_visit([&](data &d) {
      utils.eq(d.x, 20, "wrong value");
      utils.eq(d.y, 40, "wrong value");
    }),
                  "Object is not visited");

    // 15 strong
    // but default lifetime ~3-5
    for (int i{0}; i < 15; ++i) {
      pool.advance();
    }

    utils.rq_false(persist.get().try_visit([&](data &d) {}), "Use after free");

    builder.memory.persist<data>(uid);

    persist.init_if_not([]() { return data{.x = 100, .y = 100}; });

    for (int i{0}; i < 15; ++i) {
      pool.advance();
      // update lifetime
      builder.memory.persist<data>(uid);
    }

    persist.get().try_visit([&](data &d) {
      utils.eq(d.x, 100, "wrong value");
      utils.eq(d.y, 100, "wrong value");
    });

    for (int i{0}; i < 15; ++i) {
      pool.advance();
      // update lifetime
      builder.memory.persist<wrong_type>(uid);
    }

    utils.rq_false(persist.get().try_visit([&](data &d) {}), "Use after free");

    utils.rq_false(persist.get().try_visit([&](wrong_type &d) {}),
                   "wrong type visit");
  };

} _{};

}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(false); };
