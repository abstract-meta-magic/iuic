// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.env;
import std;
import iuic.test;
import iuic.underlying;
import :persist.state;

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::env::persist[state]"};

  // need
  // using test_type = iuic::test::async;

  void body(iuic::test::utils utils) {
    static constexpr iuic::state::decl state_1;
    static constexpr iuic::state::decl state_2;
    static constexpr iuic::state::decl state_3;
    static constexpr iuic::state::decl state_4;
    static constexpr iuic::state::decl state_5;
    static constexpr iuic::state::decl state_6;

    [&]<typename T = iuic::environment::persist_state_storage>() {
      if constexpr (iuic::cenv::logic("iuic::debug.api").value_or(false)) {
        iuic::advance::pool pool;
        T storage;
        storage.rebind(pool);

        iuic::units::uid uid_1{456718}; // rand
        iuic::units::uid uid_2{788476}; // rand

        auto accessor_1 = storage.access(uid_1);

        auto &slots = storage.get_pool();
        auto &gen = storage.get_generation();

        auto index_1 = storage.get_index_debug(uid_1);
        auto index_2 = storage.get_index_debug(uid_2);
        utils.neq(index_1, index_2, "validate index");

        utils.rq_false(accessor_1.has(state_1), "has state");
        utils.rq_false(accessor_1.has(state_2), "has state");
        utils.rq_false(accessor_1.has(state_3), "has state");

        accessor_1.attach(state_1);
        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_false(accessor_1.has(state_2), "has state");
        utils.rq_false(accessor_1.has(state_3), "has state");

        accessor_1.attach(state_2);
        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_true(accessor_1.has(state_2), "has state");
        utils.rq_false(accessor_1.has(state_3), "has state");

        accessor_1.attach(state_3);
        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_true(accessor_1.has(state_2), "has state");
        utils.rq_true(accessor_1.has(state_3), "has state");

        for (int i{0}; i < 10; i++) {
          pool.advance();
        }

        utils.rq_false(accessor_1.has(state_1), "state drop");
        utils.rq_false(accessor_1.has(state_2), "state drop");
        utils.rq_false(accessor_1.has(state_3), "state drop");

        // dynamic
        accessor_1.attach(state_1);
        accessor_1.attach(state_2);
        accessor_1.attach(state_3);
        accessor_1.attach(state_4);
        accessor_1.attach(state_5);
        accessor_1.attach(state_6);

        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_true(accessor_1.has(state_2), "has state");
        utils.rq_true(accessor_1.has(state_3), "has state");
        utils.rq_true(accessor_1.has(state_4), "has state dynamic");
        utils.rq_true(accessor_1.has(state_5), "has state dynamic");
        utils.rq_true(accessor_1.has(state_6), "has state dynamic");

        utils.eq(slots[index_1].dynamic_data.size(), 3, "check dynamic");

        for (int i{0}; i < 10; i++) {
          pool.advance();
        }
        utils.eq(slots[index_1].dynamic_data.size(), 0, "dynamic drop");

        accessor_1.attach(state_1);
        accessor_1.attach(state_2);
        accessor_1.attach(state_3);
        accessor_1.attach(state_4);
        accessor_1.attach(state_5);
        accessor_1.attach(state_6);

        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_true(accessor_1.has(state_2), "has state");
        utils.rq_true(accessor_1.has(state_3), "has state");
        utils.rq_true(accessor_1.has(state_4), "has state dynamic");
        utils.rq_true(accessor_1.has(state_5), "has state dynamic");
        utils.rq_true(accessor_1.has(state_6), "has state dynamic");

        accessor_1.detach(state_1);
        accessor_1.detach(state_2);
        accessor_1.detach(state_3);
        accessor_1.detach(state_4);
        accessor_1.detach(state_5);
        accessor_1.detach(state_6);
        utils.eq(slots[index_1].static_data[0], nullptr, "check detach");
        utils.eq(slots[index_1].static_data[1], nullptr, "check detach");
        utils.eq(slots[index_1].static_data[2], nullptr, "check detach");
        utils.eq(slots[index_1].dynamic_data.size(), 0, "check detach");

        // two uids
        auto accessor_2 = storage.access(uid_2);

        accessor_1.attach(state_1);
        accessor_2.attach(state_2);
        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_true(accessor_2.has(state_2), "has state");
        utils.rq_false(accessor_1.has(state_2), "has state");
        utils.rq_false(accessor_2.has(state_1), "has state");

        utils.eq(slots[index_1].static_data[0], state_1, "check raw");
        utils.eq(slots[index_2].static_data[0], state_2, "check raw");

        utils.eq(slots[index_1].generation, storage.get_generation(),
                 "check gen");

        utils.eq(slots[index_2].generation, storage.get_generation(),
                 "check gen");

        pool.advance();

        utils.eq(slots[index_1].generation, storage.get_generation() - 2,
                 "check gen");

        utils.eq(slots[index_2].generation, storage.get_generation() - 2,
                 "check gen");

        storage.get_generation() = std::numeric_limits<std::uint32_t>::max();

        accessor_1.update_lifetime();
        accessor_2.update_lifetime();

        pool.advance();

        utils.eq(storage.get_generation() % 2, 1, "check stable overflow");

        storage.get_generation() = std::numeric_limits<std::uint32_t>::max();

        accessor_1.update_lifetime();
        accessor_2.update_lifetime();

        utils.eq(slots[index_1].generation, storage.get_generation(),
                 "check gen");

        utils.eq(slots[index_2].generation, storage.get_generation(),
                 "check gen");

        // generation wrap
        for (int i{0}; i < 20; i++) {
          pool.advance();
          accessor_1.update_lifetime();
          accessor_2.update_lifetime();
        }

        utils.rq_true(accessor_1.has(state_1), "has state");
        utils.rq_true(accessor_2.has(state_2), "has state");

        storage.get_generation() =
            std::numeric_limits<std::uint32_t>::max(); // gen + 2

        accessor_1.update_lifetime();
        accessor_2.update_lifetime();

        // generation wrap
        for (int i{0}; i < 20; i++) {
          pool.advance();
        }

        utils.rq_false(accessor_1.has(state_1), "has state");
        utils.rq_false(accessor_2.has(state_2), "has state");

      } else {
        // write message
      }
    }();
  };
} _{};
} // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); };
