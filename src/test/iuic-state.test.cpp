// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

module iuic.core;
import iuic.underlying;

import :state;
using namespace iuic::test;

template ticket usecase<unit<[](utils &test) {
  // body
  std::println("in test case");
  test.eq();

  using namespace iuic::state;

  constexpr auto &a = decl::unique_instance();
  constexpr auto &b = decl::unique_instance();

  struct Data {};

  auto spec = machine::spec<
      machine::transition_graph<machine::transition{a, b, true}>{}, Data>{};
}>{}>;
