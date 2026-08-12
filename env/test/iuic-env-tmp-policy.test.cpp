// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.env;
import std;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::env::tmp[policy]"};
  void body(iuic::test::utils utils) {
    // todo
    [&]<typename T = iuic::environment::policy_tmp_registry>() {
      if constexpr (iuic::cenv::logic("iuic::debug.api").value_or(false)) {
        iuic::advance::pool pool;
        T pr{pool};

        enum class test_policy_1 {
          unset,
          policy_1,
          policy_2,
        };
        enum class test_policy_2 {
          unset,
          policy_1,
          policy_2,
        };

        iuic::units::uid uid_1{4758822274}; // rand
        iuic::units::uid uid_2{7158612472}; // rand

        pr.set(uid_1, test_policy_1::policy_2);

        utils.eq(pr.template get<test_policy_1>(uid_1), test_policy_1::policy_2,
                 "test policy set");

        utils.eq(pr.template get<test_policy_2>(uid_1), test_policy_2::unset,
                 "test policy set");

        for (int i{0}; i < 10; ++i) {
          pool.advance(); // lifetime ~3
        }

        utils.eq(pr.template get<test_policy_1>(uid_1), test_policy_1::unset,
                 "test policy set");

        // ## force rehash
        pr.set(uid_1, test_policy_1::policy_1);

        std::size_t size_dump = pr.get_size_debug();

        pr.get_load_debug() = size_dump;

        pr.set(uid_1, test_policy_2::policy_2);

        utils.eq(pr.template get<test_policy_1>(uid_1), test_policy_1::policy_1,
                 "test policy set");

        utils.eq(pr.template get<test_policy_2>(uid_1), test_policy_2::policy_2,
                 "test policy set");

        utils.neq(size_dump, pr.get_size_debug(), "check rehash");
        utils.eq(pr.get_load_debug(), 2, "check load");
      } else {
        utils.rq_true(false, "test required iuic::debug.api");
      }
    }();
  }
} _{};

} // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(); }
