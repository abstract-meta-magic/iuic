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
  static constexpr std::string_view name{"state"};

  // other info

  void body(iuic::test::utils utils) {
    advance::pool pool;
    environment::persist penv{pool};
    environment::tmp tenv{pool};

    tree::flat_unordered_type<scheme::sketch::value_t> tree;

    scheme::builder builder{penv, tenv, {tree.root()}};

    static iuic::state::decl state_1;
    static iuic::state::decl state_2;

    units::uid uid{547665327}; // rand

    builder.state.attach(uid, state_1);
    utils.rq_true(builder.state.has(uid, state_1), "has [state_1]");
    utils.rq_false(builder.state.has(uid, state_2), "has [state_2]");

    builder.state.detach(uid, state_1);
    utils.rq_false(builder.state.has(uid, state_1), "has [state_1]");

    builder.state.attach(uid, state_1);
    builder.state.attach(uid, state_2);
    utils.rq_true(builder.state.has(uid, state_1), "has [state_1]");
    utils.rq_true(builder.state.has(uid, state_2), "has [state_2]");

    // state lifetime ~3-5
    for (int i{0}; i < 10; ++i) {
      pool.advance();
    }
    utils.rq_false(builder.state.has(uid, state_1), "has [state_1]");
    utils.rq_false(builder.state.has(uid, state_2), "has [state_2]");
  }
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::registry::instance().run(false); };
