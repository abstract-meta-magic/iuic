// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.core;
import std;
import iuic.underlying;
import :scheme.compute;

// decompose
namespace {
using namespace iuic;
static constexpr inline units::upixel
adapt_to_pixel(const units::ui::adaptive::unit &value, units::upixel psize,
               units::ui::size viewport, units::upixel segment) {
  return std::visit(
      [&]<typename type>(const type &obj) -> units::upixel {
        if constexpr (std::same_as<type, units::percent>) {
          return psize * obj;
        } else if constexpr (std::same_as<type, units::vh>) {
          return viewport.h * obj;
        } else if constexpr (std::same_as<type, units::vw>) {
          return viewport.w * obj;
        } else if constexpr (std::same_as<type, units::segment>) {
          return segment * obj;
        } else if constexpr (std::same_as<type, units::upixel>) {
          return obj;
        }
        return units::upixel{0};
      },
      value);
};

static constexpr inline units::ui::area
calc_root_child_area(const units::ui::adaptive::size &m, style::value style,
                     units::ui::size viewport, units::upixel segment) {
  units::ui::area res{};

  res.borderless.w = adapt_to_pixel(m.width, viewport.w, viewport, segment);
  res.borderless.h = adapt_to_pixel(m.height, viewport.h, viewport, segment);

  // border
  auto &shape = style.get_shape();

  auto border_left =
      adapt_to_pixel(shape.border.left, viewport.w, viewport, segment);
  auto border_right =
      adapt_to_pixel(shape.border.right, viewport.w, viewport, segment);
  auto border_top =
      adapt_to_pixel(shape.border.top, viewport.h, viewport, segment);
  auto border_bottom =
      adapt_to_pixel(shape.border.bottom, viewport.h, viewport, segment);

  res.bordered.w = res.borderless.w + border_left + border_right;
  res.bordered.h = res.borderless.h + border_top + border_bottom;
  res.borderless.x = res.borderless.x + border_left;
  res.borderless.y = res.borderless.y + border_top;

  return res;
};

void measure(environment::tmp &tenv, scheme::blueprint &bp) {

  auto range =
      tree::iterator_range_for{bp, tree::iterator_type<tree::base_iterator>{},
                               tree::tag::reverse_levelorder{}};

  for (auto el : range) {
    auto ait = tree::access_iterator{el};

    std::visit(
        [&](auto obj) {
          if constexpr (std::same_as<decltype(obj), const layout::frame *>) {

            if (auto res = obj->measure(layout::measure::frame_utils{
                    tenv, tree::base_iterator{ait}})) {
              ait->is = ui::element::measured{res.value()};
            } else {
              ait->is = ui::element::discarded{};
            };
          } else if constexpr (std::same_as<decltype(obj),
                                            const layout::text *>) {
            if (auto res = obj->measure(layout::measure::text_utils{
                    tenv, ait->text, tree::base_iterator{ait}})) {
              ait->is = ui::element::measured{res.value()};
            } else {
              ait->is = ui::element::discarded{};
            };
          } else {
            ait->is = ui::element::discarded{};
          };
        },
        ait->type);
  };
};

void arrange_root(environment::tmp &tenv, scheme::blueprint &bp) {
  auto root_ch = tree::childs_of(bp.root());

  iuic::units::ui::position cursor{units::pixel{0}, units::pixel{0}};
  iuic::units::upixel margin{0};

  for (auto el : tree::iterator_range_for{
           root_ch, tree::iterator_type<tree::base_iterator>{}}) {
    // DO JOB
    auto ait = tree::access_iterator{el};

    if (not std::holds_alternative<ui::element::measured>(ait->is)) {
      // TODO: normal handling
      continue;
    }

    auto area = calc_root_child_area(
        std::get<ui::element::measured>(ait->is), tenv.style.get(ait->sid),
        tenv.meta.viewport_size, tenv.meta.segment_size);

    area.bordered.y = area.bordered.y + cursor.y;
    area.borderless.y = area.borderless.y + cursor.y;

    // TODO : calc margin

    ait->is = ui::element::placed{area};

    cursor.y = cursor.y + area.bordered.h;
  };
};

void arrange(environment::tmp &tenv, scheme::blueprint &bp) {
  arrange_root(tenv, bp);

  for (auto el : tree::iterator_range_for{
           bp, tree::iterator_type<tree::base_iterator>{}}) {
    auto ait = tree::access_iterator{el};

    // TODO : DISCARTED | VIRTUALIZED
    if (not std::holds_alternative<ui::element::placed>(ait->is)) {
      if (std::holds_alternative<ui::element::virtualized>(ait->is)) {
        // do virtualize
      } else {
        ait->is = ui::element::discarded{};

        for (auto ch : tree::iterator_range_for{
                 tree::childs_of(ait),
                 tree::iterator_type<tree::access_iterator>{}}) {
          ch->is = ui::element::discarded{};
        };
      }
      continue;
    }

    std::visit(
        [&](auto obj) {
          if constexpr (std::same_as<decltype(obj), const layout::frame *>) {
            if (iuic::tree::root_iterator rit{ait}; ++rit) {
              ait->clip = iuic::tree::access_iterator{rit}->clip;
            }

            if (not obj->arrange(iuic::layout::arrange::frame_utils{
                    tenv, tree::base_iterator{ait}})) {
              ait->is = ui::element::discarded{};
              return;
            };
          } else {
            if (auto res = obj->arrange(iuic::layout::arrange::text_utils{
                    tenv, ait->text, tree::base_iterator{ait}})) {
              tenv.text.get(ait->text).value = res.value();
            } else {
              ait->is = ui::element::discarded{};
              tenv.text.get(ait->text).value = text::data::corrupted{};
            }
          }
        },
        tree::access_iterator{el}->type);
  };
};

}; // namespace

namespace iuic::scheme {

bool compute(blueprint &bp, environment::tmp &tenv,
             environment::persist &penv) {

  measure(tenv, bp);
  arrange(tenv, bp);

  return true;
};
}; // namespace iuic::scheme
