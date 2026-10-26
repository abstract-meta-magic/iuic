// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.layout;
import std;
import iuic.underlying;
import iuic.test;

namespace {
struct test_1 : iuic::test::unit<test_1> {
  static constexpr std::string_view name{"iuic::layout[base]"};

  void body(iuic::test::utils utils) {
    // TODO: write tests
  };
} _1;

}; // namespace

extern "C++" int main() { return iuic::test::run(); };
