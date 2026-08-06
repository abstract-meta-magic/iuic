// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.env;
import std;
import iuic.test;
import :tmp.exception;

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::env::tmp[memory]"};
  void body(iuic::test::utils utils) {
    //
    iuic::advance::pool pool;
    iuic::environment::tmp_memory mem{pool};

    auto res_1 = mem.allocate<int>();

    utils.eq(res_1.size(), 1, "alloc");

    auto res_2 = mem.allocate<int>(8);

    utils.eq(res_2.size(), 8, "alloc");

    struct bigdata {
      std::byte byte[iuic::cenv::num("iuic::tmp_buff_size")
                         .value_or(1024 * 1024 * 4) /
                     2];
    };

    utils.ex_catch<iuic::exception::tmp_buffer_overflow>(
        [&]() { mem.allocate<bigdata>(2); }, "try overflow");
    pool.advance();

    auto res_3 = mem.allocate<bigdata>(2);
    utils.eq(res_3.size(), 2, "alloc");
  };
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
