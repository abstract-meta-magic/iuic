// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.asset;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {

  static constexpr std::string_view name{"iuic::asset::map"};

  void body(iuic::test::utils utils) {
    iuic::container<iuic::asset::map::fixed<int, std::string_view, 1024>> c;

    auto access = c.access(7);

    utils.rq_false(access.has_value(), "has value");

    utils.rq_true(std::move(access).set("penis"), "set");

    utils.rq_true(access.has_value(), "has value");

    iuic::container<iuic::asset::map::fixed<iuic::asset::declaration,
                                            std::string_view, 1024>>
        c2;

    std::vector<std::string> hash_name;

    for (std::size_t i{0}; i < 300; ++i) {
      hash_name.push_back(std::format("name#{}", i));
    };

    std::size_t i{0};
    for (auto &name : hash_name) {
      c2.access(iuic::asset::declaration{name}).set(name);
    }

    for (auto &name : hash_name) {
      utils.eq(c2.access(iuic::asset::declaration{name}).get(), name,
               "check name");
    }

    for (auto &name : hash_name) {
      c2.access(iuic::asset::declaration{name}).reset();
      auto access = c2.access("sus");
    }

    for (auto &name : hash_name) {
      utils.rq_false(c2.access(iuic::asset::declaration{name}).has_value(),
                     "has value");
    }

    // modify
  };

} _;
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
