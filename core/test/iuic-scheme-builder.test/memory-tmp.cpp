// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.core;
import iuic.test;
import iuic.underlying;
import iuic.env;
import :scheme.builder;

#include "dummy-layout.inc"

namespace {
using namespace iuic;

struct test_basic : iuic::test::unit<test_basic> {
  static constexpr std::string_view name{"memory::tmp(basic)"};

  void body(iuic::test::utils utils) {
    advance::pool pool;
    environment::persist penv{pool};
    environment::tmp tenv{pool};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, {tree.root()}};

    struct data {
      int x, y;
    };

    auto &test_data = builder.memory.tmp(data{.x = 2, .y = 4});

    utils.eq(test_data.x, 2, "wrong value");
    utils.eq(test_data.y, 4, "wrong value");
  }
} basic{};

struct test_ex : iuic::test::unit<test_ex> {
  static constexpr std::string_view name{"memory::tmp(ex)"};

  // other info

  void body(iuic::test::utils utils) {
    advance::pool pool;
    environment::persist penv{pool};
    environment::tmp tenv{pool};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, {tree.root()}};

    struct large_type {
      std::byte data[iuic::cenv::num("iuic::env.tmp_buffer_size")
                         .value_or(1024 * 1024 * 4) /
                     4];
    };

    utils.ex_catch<iuic::exception::tmp_buffer_overflow>(
        [&]() {
          for (; true;) {
            builder.memory.tmp(large_type{});
          }
        },
        "No exception throw");

    pool.advance();

    utils.ex_nothrow([&]() { builder.memory.tmp(large_type{}); },
                     "No reset data");
  }
} ex{};

} // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(false); };
