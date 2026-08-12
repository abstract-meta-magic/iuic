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
    iuic::advance::pool pool;
    iuic::environment::tmp tenv{pool};
    tenv.meta.viewport_size = {iuic::units::upixel{40},
                               iuic::units::upixel{40}};
    tenv.meta.segment_size = iuic::units::upixel{16};

    static constexpr iuic::style::decl style{
        .shape = {iuic::units::upixel{70}, iuic::units::upixel{70}}};
    static constexpr iuic::style::decl pareng_style{
        .shape = {iuic::units::upixel{140}, iuic::units::upixel{140}}};

    auto sid = tenv.style.make(style);
    auto psid = tenv.style.make(pareng_style);
    auto style_value = tenv.style.get(sid);
    auto parent_style_value = tenv.style.get(psid);

    iuic::tree::flat_unordered_type<iuic::layout::element> t;

    iuic::tree::insert_iterator iit{t.root()};

    //   R
    //   *
    //   *
    // * * *
    iit = iit.at({.style = parent_style_value});
    // target
    iit = iit.at({.style = style_value});
    // ch
    iit = iit.at({.style = style_value});
    iit.to({.style = style_value});
    iit.to({.style = style_value});

    iuic::layout::tree_type tree{iuic::tree::move_iterator{t.root()}};

    //   R
    //   * <- begin
    //   * <- childs_of
    // * * *
    auto target = iuic::tree::childs_of(tree.begin());

    std::size_t to_root{0}; // self + 1

    for (auto rit{iuic::tree::root_iterator{target}};
         not rit.is_root() && rit.valid(); ++rit) {
      ++to_root;
    }

    // self + 1
    utils.eq(to_root, 2, "check position");

    std::size_t ch_count{0};
    for (auto _ :
         iuic::tree::iterator_range_for{iuic::tree::childs_of(target)}) {
      ++ch_count;
    }
    utils.eq(ch_count, 3, "check child count");

    // DO JOB
    iuic::layout::utils_base b{tenv, target};

    ch_count = 0;
    for (auto ch : b.childs()) {
      ++ch_count;
    };
    utils.eq(ch_count, 3, "check child count");

    utils.eq(tenv.meta.viewport_size, b.root_size(), "root size");
    utils.eq(tenv.meta.segment_size, b.segment_size(), "segment size");
    //
    utils.eq(b.self(), target, "validate self");
    utils.eq(b.parent(), ++iuic::tree::root_iterator{target},
             "validate parent");
    //
    auto to_pixel = [](const iuic::units::ui::adaptive::unit &unit) {
      return std::visit(
          []<typename type>(const type &px) -> iuic::units::upixel {
            if constexpr (std::same_as<type, iuic::units::upixel>) {
              return px;
            } else {
              throw std::logic_error{"invalid type"};
            };
          },
          unit);
    };

    auto &self_shape = b.style[b.self()].get_shape();
    utils.ex_nothrow(
        [&]() {
          utils.eq(to_pixel(self_shape.size.width), iuic::units::upixel{70},
                   "check shape size");
          utils.eq(to_pixel(self_shape.size.height), iuic::units::upixel{70},
                   "check shape size");

          auto &parent_shape = b.style[b.parent()].get_shape();
          utils.eq(to_pixel(parent_shape.size.width), iuic::units::upixel{140},
                   "check shape size");
          utils.eq(to_pixel(parent_shape.size.height), iuic::units::upixel{140},
                   "check shape size");
        },
        "invalid convertion");
  }
} base{};

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{"iuic::layout[frame]"};

  void body(iuic::test::utils utils) {
    iuic::advance::pool pool;
    iuic::environment::tmp tenv{pool};

    static constexpr iuic::style::decl style{
        .shape = {iuic::units::upixel{70}, iuic::units::upixel{70}}};
    static constexpr iuic::style::decl pareng_style{
        .shape = {iuic::units::upixel{140}, iuic::units::upixel{140}}};

    auto sid = tenv.style.make(style);
    auto psid = tenv.style.make(pareng_style);
    auto style_value = tenv.style.get(sid);
    auto parent_style_value = tenv.style.get(psid);

    iuic::units::ui::area test_area{
        .bordered = iuic::units::ui::rect{.x = iuic::units::pixel{20},
                                          .y = iuic::units::pixel{20},
                                          .w = iuic::units::upixel{20},
                                          .h = iuic::units::upixel{20}},
        .borderless = iuic::units::ui::rect{.x = iuic::units::pixel{20},
                                            .y = iuic::units::pixel{20},
                                            .w = iuic::units::upixel{20},
                                            .h = iuic::units::upixel{20}}};

    iuic::tree::flat_unordered_type<iuic::layout::element> t;

    iuic::tree::insert_iterator iit{t.root()};

    //   R
    //   *
    //   *
    // * * *
    iit = iit.at({.style = parent_style_value});
    // target
    iit = iit.at({.measure{iuic::units::upixel{40}, iuic::units::upixel{40}},
                  .arrange = test_area,
                  .style = style_value});
    // ch
    iit = iit.at({.style = style_value});
    iit.to({.style = style_value});
    iit.to({.style = style_value});

    iuic::layout::tree_type tree{iuic::tree::move_iterator{t.root()}};

    //   R
    //   * <- begin
    //   * <- childs_of
    // * * *
    auto target = iuic::tree::childs_of(tree.begin());

    iuic::layout::arrange::frame_utils arrange{tenv, target};

    auto to_pixel = [](const iuic::units::ui::adaptive::unit &unit) {
      return std::visit(
          []<typename type>(const type &px) -> iuic::units::upixel {
            if constexpr (std::same_as<type, iuic::units::upixel>) {
              return px;
            } else {
              throw std::logic_error{"invalid type"};
            };
          },
          unit);
    };

    auto &m = arrange.measure[arrange.self()];
    utils.ex_nothrow(
        [&]() {
          utils.eq(to_pixel(m.width), iuic::units::upixel{40}, "check");
          utils.eq(to_pixel(m.height), iuic::units::upixel{40}, "check");
        },
        "check measure");

    utils.eq(arrange.self_area(), test_area, "check self_area");

    for (auto ch : arrange.childs()) {
      arrange.apply_element(ch, test_area);
    }

    utils.eq(test_area, test_area, "base eq");

    for (auto it : iuic::tree::iterator_range_for{
             iuic::tree::childs_of(target),
             iuic::tree::iterator_type<iuic::tree::access_iterator>{}}) {
      auto &area = it->arrange;
      utils.eq(area, test_area, "check apply");
    }
  };
} frame{};

struct test_3 : iuic::test::unit<test_3> {
  static constexpr std::string_view name{"iuic::layout[text]"};

  void body(iuic::test::utils utils) {
    iuic::advance::pool pool;
    iuic::environment::tmp tenv{pool};
    tenv.meta.viewport_size = {iuic::units::upixel{40},
                               iuic::units::upixel{40}};
    tenv.meta.segment_size = iuic::units::upixel{16};

    static constexpr iuic::style::decl style{
        .shape = {iuic::units::upixel{70}, iuic::units::upixel{70}}};
    static constexpr iuic::style::decl pareng_style{
        .shape = {iuic::units::upixel{140}, iuic::units::upixel{140}}};

    auto sid = tenv.style.make(style);
    auto psid = tenv.style.make(pareng_style);
    auto style_value = tenv.style.get(sid);
    auto parent_style_value = tenv.style.get(psid);

    iuic::tree::flat_unordered_type<iuic::layout::element> t;

    iuic::tree::insert_iterator iit{t.root()};

    //   R
    //   *
    //   *
    // * * *
    iit = iit.at({.style = parent_style_value});
    // target
    iit = iit.at({.style = style_value});
    // ch
    iit = iit.at({.style = style_value});
    iit.to({.style = style_value});
    iit.to({.style = style_value});

    iuic::layout::tree_type tree{iuic::tree::move_iterator{t.root()}};

    //   R
    //   * <- begin
    //   * <- childs_of
    // * * *
    auto target = iuic::tree::childs_of(tree.begin());

    // token
    std::vector<iuic::text::glyph::id_t> glyphs{4, 4, 4, 2, 2, 2};

    iuic::text::raw::token tk{.atlas_id = 4, .glyphs = glyphs};
    iuic::layout::arrange::text_utils text{tenv, std::span{&tk, 1}, target};
    // fill placment
    std::vector<iuic::text::present::placement> placement{
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{2}},
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{4}},
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{6}},
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{8}},
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{10}},
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{12}},
        iuic::text::present::placement{.id = 2,
                                       .offset = iuic::units::pixel{14}},
    };

    // fill token
    iuic::text::present::token pt{.atlas_id = tk.atlas_id,
                                  .placement =
                                      text.commit(std::move(placement))};

    std::vector<iuic::text::present::token> tks{pt};
    // commit token
    auto result = text.commit(std::move(tks));

    utils.rq_false(result.empty(), "check result");

    for (auto &tk : result) {
      utils.eq(tk.atlas_id, 4, "check atlas id");
      iuic::units::pixel offset{2};
      for (auto &pl : tk.placement) {
        utils.eq(pl.offset, offset, "check offset");
        utils.eq(pl.id, 2, "check glyph");
        offset = offset + iuic::units::pixel{2};
      }
    }
  }
} text{};
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
