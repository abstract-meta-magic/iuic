// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.layout;
import std;
import iuic.test;
import iuic.underlying;

namespace {
using namespace iuic::units::literals;

struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::layout[frame]"};

  static constexpr auto test_area = iuic::units::ui::area{
      .bordered{.w = 40_upx, .h = 40_upx, .x = 0_px, .y = 0_px},
      .borderless{.w = 40_upx, .h = 40_upx, .x = 0_px, .y = 0_px}};

  static void body(iuic::test::utils utils) {
    // TODO: write tests
    iuic::layout::tree tree;

    auto ii = iuic::iterator::insert{tree.root()};

    ii[iuic::ui::element{}];
    ii[iuic::ui::element{}];
    ii[iuic::ui::element{}];

    std::size_t c{0};
    for (auto _ : iuic::iterator::range::from(tree)) {
      ++c;
    };
    utils.eq(c, 3, "size");

    iuic::advance::pool p;
    iuic::environment::tmp tenv{p};

    iuic::layout::measure::frame_utils fm_utils{tenv, tree.begin()};
    // apply measure
    iuic::iterator::access ait{iuic::tree::childs_of(tree.begin())};
    ait->is = iuic::ui::element::measured{40_upx, 40_upx};

    auto ch = fm_utils.childs().begin();

    utils.eq(ch, ait, "child");
    utils.eq(fm_utils.self(), tree.begin(), "self");
    utils.eq(fm_utils.parent(), tree.root(), "root");

    iuic::layout::arrange::frame_utils fa_utils{tenv, tree.begin()};
    iuic::iterator::access{tree.begin()}->is =
        iuic::ui::element::placed{test_area};

    utils.eq(fa_utils.self_area(), test_area, "area");
    //
    {
      iuic::iterator::basic ch = iuic::tree::childs_of(tree.root());
      iuic::iterator::access{ch}->is =
          iuic::ui::element::measured{40_upx, 40_upx};

      fa_utils.apply_element(ch, test_area);

      iuic::iterator::access ait{ch};
      utils.rq_true(std::holds_alternative<iuic::ui::element::placed>(ait->is),
                    "set");
    }

    // fa_utils.make_clipzone();
  };

} _{};

}; // namespace

extern "C++" int main() {
  // return 0;
  return iuic::test::run();
};
