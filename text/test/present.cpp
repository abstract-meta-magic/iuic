// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.text;
import std;
import iuic.test;

namespace {

struct test_1 : iuic::test::unit<test_1> {
  static constexpr std::string_view name{"iuic::text[present.fill]"};

  void body(iuic::test::utils utils) {
    iuic::text::present::tool pt;

    iuic::units::ui::position baseline;
    iuic::units::upixel line_height;
    iuic::units::pixel offset;
    iuic::text::gid g{10};

    auto pl = pt.create_placemnt(baseline, line_height);

    pl.append(g, offset);
  };
} _1;

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{"iuic::text[present.logical]"};

  void body(iuic::test::utils utils) {
    iuic::text::present::tool pt;

    iuic::units::ui::position baseline;
    iuic::units::upixel line_height;
    iuic::units::pixel offset;
    iuic::text::gid g{10};
    iuic::text::lsgid group{24};

    auto pl = pt.create_placemnt(baseline, line_height, group);

    pl.append(g, offset);

    auto placed = pt.compile();

    utils.eq_then(placed.by_placements().size(), 1, "placement size", [&] {
      auto placement = placed.by_placements().at(0);

      utils.eq(placement.get_logical_group(), group, "logical group");
    });
  };
} _2;

struct test_3 : iuic::test::unit<test_3> {
  static constexpr std::string_view name{"iuic::text[present.semantic]"};

  void body(iuic::test::utils utils) {

    iuic::text::present::tool pt;

    iuic::units::ui::position baseline;
    iuic::units::upixel line_height;
    iuic::units::pixel offset;
    iuic::text::gid g{10};
    iuic::text::sgid group{24};

    auto pl = pt.create_placemnt(baseline, line_height, group);

    pl.append(g, offset);

    auto placed = pt.compile();

    utils.eq_then(placed.by_placements().size(), 1, "placement size", [&] {
      auto placement = placed.by_placements().at(0);

      utils.eq(placement.get_semantic_group(), group, "semantic group");
    });
  };
} _3;

}; // namespace

extern "C++" int main() { return iuic::test::run(); };
