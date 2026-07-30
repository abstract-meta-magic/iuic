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
  static constexpr std::string_view name{"policy"};

  // other info

  void body(iuic::test::utils utils) {
    advance::pool pool;
    environment::persist penv{pool};
    environment::tmp tenv{pool};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, {tree.root()}};

    enum class policy_1 {
      unset,
      state_1,
      state_2,
      state_3,
    };

    enum class policy_2 {
      unset,
      state_1,
      state_2,
      state_3,
    };

    units::uid uid{783857232};
    static dummy_layout layout;

    //
    builder.policy.set(uid, policy_1::state_1);

    auto get_policy_1 = builder.policy.get<policy_1>(uid);
    utils.eq(policy_1::state_1, get_policy_1, "policy eq");
    auto unset_policy = builder.policy.get<policy_2>(uid);
    utils.eq(policy_2::unset, unset_policy, "unset policy");

    pool.advance();

    auto get_policy_2 = builder.policy.get<policy_1>(uid);
    utils.eq(policy_1::unset, get_policy_2, "unset after advance");

    builder.policy.set(uid, policy_1::state_1);
    builder.policy.set(uid, policy_2::state_2);
    auto get_policy_3 = builder.policy.get<policy_1>(uid);
    auto get_policy_4 = builder.policy.get<policy_2>(uid);
    utils.eq(policy_1::state_1, get_policy_3, "policy eq");
    utils.eq(policy_2::state_2, get_policy_4, "policy eq");
    //
  }
} _{};
} // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(false); };
