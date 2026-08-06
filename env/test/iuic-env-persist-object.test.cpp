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
    // TODO:
    // invalide arena.get(id)
    // double free
    // large alloc > 256 bytes

    [&]<typename T = iuic::environment::persist_object_storage_arena>() {
      if constexpr (iuic::cenv::logic("iuic::debug.api").value_or(false)) {
        T arena;
        auto &data = arena.data();
        using enum iuic::environment::persist_object_storage_arena::pool_index;

        // 16[0],32[1],64[2],...
        utils.eq(data[for_64_byte].chunks[0].memory, nullptr,
                 "validate chunk memory");

        auto id = arena.allocate(48);

        // 16[0],32[1],64[2],...
        utils.neq(data[for_64_byte].chunks[0].memory, nullptr,
                  "validate chunk memory");
        utils.eq(data[for_32_byte].chunks[0].memory, nullptr,
                 "validate chunk memory");
        utils.eq(data[for_16_byte].chunks[0].memory, nullptr,
                 "validate chunk memory");

        // 16[0],32[1],64[2],...
        utils.eq(data[for_64_byte].chunks[0].free_count,
                 data[for_64_byte].chunks[0].capacity - 1,
                 "validate chunk size");

        auto *ptr = arena.get(id);

        utils.neq(ptr, nullptr, "validate ptr");

        arena.deallocate(id);

        // 16[0],32[1],64[2],...
        utils.eq(data[for_64_byte].chunks[0].free_count,
                 data[for_64_byte].chunks[0].capacity, "validate chunk size");

        std::vector<iuic::environment::arena_id> alloc_ids;
        for (; data[for_64_byte].chunks[0].free_count != 0;) {
          alloc_ids.push_back(arena.allocate(64));
        }

        utils.eq(data[for_64_byte].chunks[0].free_count, 0, "check full");

        arena.allocate(64);

        utils.eq(data[for_64_byte].chunks[1].free_count,
                 data[for_64_byte].chunks[1].capacity - 1, "new chunk usage");

        std::size_t deallocate_count = data[for_64_byte].chunks[0].capacity / 2;
        for (int i{0}; i < deallocate_count; ++i) {
          arena.deallocate(alloc_ids.at(i));
        }

        utils.eq(data[for_64_byte].chunks[0].free_count, deallocate_count,
                 "old chunk usage");
        auto id_1 = arena.allocate(64);
        utils.neq(arena.get(id_1), nullptr, "validate ptr");

        utils.eq(data[for_64_byte].chunks[0].free_count, deallocate_count - 1,
                 "old chunk usage");
        utils.eq(data[for_64_byte].chunks[1].free_count,
                 data[for_64_byte].chunks[1].capacity - 1, "new chunk usage");

        for (std::size_t i{deallocate_count}; i < alloc_ids.size(); ++i) {
          utils.neq(arena.get(alloc_ids[i]), nullptr, "validate ptr");
        }
      } else {
        // warn
        utils.rq_true(false, "test required debug api");
      }
    }();
  };

} base{};

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{
      "iuic::env::persist[object_storage(pool)]"};

  void body(iuic::test::utils utils) {
    // TODO : later
    // add double alloc\ctor\dealloc
    static std::size_t counter{};
    if (true) {
      iuic::advance::pool pool;

      iuic::environment::persist_object_storage storage;

      storage.rebind(pool);

      struct data {
        int x, y;

        data(int x_, int y_) : x{x_}, y{y_} { ++counter; }
        ~data() { --counter; }
      };

      struct wrong_type {};

      iuic::units::uid uid_1{45768875214}; // rand
      iuic::units::uid uid_2{45767735216}; // rand

      auto accessor_1 =
          storage.access(uid_1, iuic::erasure::type::from<data>());

      utils.eq(accessor_1.state(), iuic::environment::object_state::non_exist,
               "non exist");

      accessor_1.reserve();
      // check state
      utils.eq(accessor_1.state(),
               iuic::environment::object_state::reserve_this_type,
               "check state");
      auto accessor_1_1 =
          storage.access(uid_1, iuic::erasure::type::from<wrong_type>());

      utils.eq(accessor_1_1.state(),
               iuic::environment::object_state::reserve_other_type,
               "check state");

      accessor_1.construct([](void *ptr) { new (ptr) data{4, 4}; });
      utils.eq(counter, 0, "check ctor before allocate");

      accessor_1.allocate();

      utils.eq(accessor_1.state(),
               iuic::environment::object_state::alive_this_type, "check state");

      // now can't controll ctor usage. ake state::alvie_tihs_type vs
      // state::allocate_this_type...
      accessor_1.construct([](void *ptr) { new (ptr) data{4, 4}; });
      // check state

      utils.eq(counter, 1, "ctor check");

      auto accessor_2 =
          storage.access(uid_2, iuic::erasure::type::from<data>());

      accessor_2.reserve();
      utils.eq(accessor_2.state(),
               iuic::environment::object_state::reserve_this_type,
               "check state");
      accessor_2.allocate();
      utils.eq(accessor_2.state(),
               iuic::environment::object_state::alive_this_type, "check state");
      accessor_2.construct([](void *ptr) { new (ptr) data{8, 8}; });

      utils.eq(counter, 2, "ctor check");

      utils.rq_true(
          iuic::erasure::visited::as_mutable{accessor_1.get()}.try_visit(
              [&](data &data) {
                //
                utils.eq(data.x, 4, "wrong value");
                utils.eq(data.y, 4, "wrong value");
              }),
          "try visit");

      utils.rq_false(
          iuic::erasure::visited::as_mutable{accessor_1_1.get()}.try_visit(
              [&](wrong_type &data) { std::println("wrong visit"); }),
          "invalid visit");

      utils.rq_true(
          iuic::erasure::visited::as_mutable{accessor_2.get()}.try_visit(
              [&](data &data) {
                //
                utils.eq(data.x, 8, "wrong value");
                utils.eq(data.y, 8, "wrong value");
              }),
          "try visit");

      // ~10 gen
      for (int i{0}; i < 10; ++i) {
        pool.advance();
      }

      utils.eq(counter, 0, "dtor check");

      utils.rq_false(
          iuic::erasure::visited::as_mutable{accessor_1.get()}.try_visit(
              [](data &) {}),
          "visit after free");
      utils.rq_false(
          iuic::erasure::visited::as_mutable{accessor_2.get()}.try_visit(
              [](data &) {}),
          "visit after free");

      accessor_1.reserve();
      accessor_1.allocate();
      accessor_1.construct([](void *ptr) { new (ptr) data{4, 4}; });
      accessor_2.reserve();
      accessor_2.allocate();
      accessor_2.construct([](void *ptr) { new (ptr) data{4, 4}; });
    }

    utils.eq(counter, 0, "dtor check");

    // check delete
  };
} pool{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
