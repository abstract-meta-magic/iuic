// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.layout;
import std;
import iuic.underlying;
import iuic.test;

namespace {
using namespace iuic::units::literals;

struct test_1 : iuic::test::unit<test_1> {
  static constexpr std::string_view name{"iuic::layout[frame]"};

  static constexpr auto test_area = iuic::units::ui::area{
      .bordered{.w = 40_upx, .h = 40_upx, .x = 0_px, .y = 0_px},
      .borderless{.w = 40_upx, .h = 40_upx, .x = 0_px, .y = 0_px}};

  void body(iuic::test::utils utils) {
    // TODO: write tests
    iuic::tree::flat_unordered_type<iuic::ui::element> make_tree;

    auto ii = iuic::tree::insert_iterator{make_tree.root()};

    ii = ii.at({});
    ii = ii.at({});
    ii = ii.at({});

    iuic::layout::tree tree{iuic::tree::move_iterator{make_tree.root()}};

    std::size_t c{0};
    for (auto _ : iuic::tree::iterator_range_for{
             tree, iuic::tree::iterator_type<iuic::tree::base_iterator>{}}) {
      ++c;
    };
    utils.eq(c, 3, "size");

    iuic::advance::pool p;
    iuic::environment::tmp tenv{p};

    iuic::layout::measure::frame_utils fm_utils{tenv, tree.begin()};
    // apply measure
    iuic::tree::access_iterator ait{iuic::tree::childs_of(tree.begin())};
    ait->is = iuic::ui::element::measured{40_upx, 40_upx};

    auto ch = fm_utils.childs().begin();

    utils.eq(ch, ait, "child");
    utils.eq(fm_utils.self(), tree.begin(), "self");
    utils.eq(fm_utils.parent(), tree.root(), "root");

    iuic::layout::arrange::frame_utils fa_utils{tenv, tree.begin()};
    iuic::tree::access_iterator{tree.begin()}->is =
        iuic::ui::element::placed{test_area};

    utils.eq(fa_utils.self_area(), test_area, "area");
    //
    {
      iuic::tree::base_iterator ch = iuic::tree::childs_of(tree.begin());
      iuic::tree::access_iterator{ch}->is =
          iuic::ui::element::measured{40_upx, 40_upx};

      fa_utils.apply_element(ch, test_area);

      iuic::tree::access_iterator ait{ch};
      utils.rq_true(std::holds_alternative<iuic::ui::element::placed>(ait->is),
                    "set");
    }

    // fa_utils.make_clipzone();
  };
} _1;

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{"iuic::layout[text]"};

  struct layout : iuic::layout::text {
    // measure
    // arrange
  };

  void body(iuic::test::utils utils) {
    // TODO: write tests
    iuic::layout::tree tree;

    // layout l;
  };
} _2;

}; // namespace

extern "C++" int main() { return iuic::test::run(); };
