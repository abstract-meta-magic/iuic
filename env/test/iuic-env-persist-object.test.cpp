// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.env;
import :persist.object;
import iuic.underlying;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{
      "iuic::env::persist[object_storage(arena)]"};

  void body(iuic::test::utils utils) {
    // do
    iuic::environment::persist_object_storage_arena arena;

    auto id = arena.allocate(48);

    auto *ptr = arena.get(id);

    utils.neq(ptr, nullptr, "ptr validation");

    if constexpr (iuic::cenv::logic("iuic::debug.api").value_or(false)) {
    } else {
      // уведомить, что проверка неполная.
    }
  };
} base{};

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{
      "iuic::env::persist[object_storage(pool)]"};

  void body(iuic::test::utils utils) {
    // do
    iuic::environment::persist_object_storage storage;

    struct data {
      int x, y;
    };

    struct wrong_type {};

    iuic::units::uid uid{45768875214}; // rand

    auto accessor = storage.access(uid, iuic::erasure::type::from<data>());

    utils.eq(accessor.state(), iuic::environment::object_state::non_exist,
             "non exist");

    if constexpr (iuic::cenv::logic("iuic::debug.api").value_or(false)) {
      //
    } else {
      // уведомить, что проверка неполная.
    }
  };
} pool{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
