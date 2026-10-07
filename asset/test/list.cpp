// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.asset;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {

  static constexpr std::string_view name{"iuic::asset::map"};

  void body(iuic::test::utils utils) {
    iuic::container<iuic::asset::free_list::fixed<int, 1024>> c;

    std::size_t inseprt_point_0 = c.insert(12);
    std::size_t inseprt_point_1 = c.insert(17);

    utils.eq(inseprt_point_0, 0, "IP-0");
    utils.eq(inseprt_point_1, 1, "IP-0");
    utils.rq_true(c.has_value(inseprt_point_0), "Has value IP-0");
    utils.rq_true(c.has_value(inseprt_point_1), "Has value IP-1");

    for (std::size_t i{0}; i < 8; ++i) {
      c.insert(2);
    };

    c.erase(inseprt_point_1);

    std::size_t inseprt_point_3 = c.insert(12);
    utils.eq(inseprt_point_3, 1, "IP-3");
    utils.rq_true(c.has_value(inseprt_point_3), "Has value IP-3");

    iuic::container<iuic::asset::free_list::fixed<int, 1024>> c2;

    for (; not c2.full(); c2.insert(2)) {
    };
    utils.rq_true(c2.has_value(1023), "check");
    // modify
  };

} _;
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
