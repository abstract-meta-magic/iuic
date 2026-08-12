// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.event;
import std;
import iuic.test;

namespace {

static constexpr iuic::event::channel test_channel{.memory = {.size = 4048}};

struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::events[alloc]"};

  void body(iuic::test::utils utils) {
    iuic::advance::pool pool;
    iuic::event::allocator<test_channel> alloc{pool};
    struct data {
      int x, y;
    };

    auto *mptr = alloc.allocate<data>();
    utils.rq_true(mptr, "check ptr");
    new (mptr) data{4, 7};

    struct big_data {
      std::byte my[test_channel.memory.size - 4];
    };

    utils.ex_catch<std::bad_alloc>([&]() { alloc.allocate<big_data>(); },
                                   "check exception");

    pool.advance();

    utils.ex_nothrow([&]() { alloc.allocate<big_data>(); }, "check advance");
  };
} alloc{};

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{"iuic::events[pool]"};

  void body(iuic::test::utils utils) {
    iuic::advance::pool pool;

    iuic::event::pool<test_channel> event_pool{pool};

    event_pool.push({});

    utils.eq(event_pool.list().size(), 1, "check push");

    pool.advance();

    utils.rq_true(event_pool.list().empty(), "check advance");
  };
} pool{};

}; // namespace

extern "C++" int main() { return iuic::test::run(); };
