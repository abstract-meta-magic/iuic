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

void measure(environment::tmp &tenv, layout::tree &bp) {

  // reverse
  auto range = iuic::iterator::range::from(iuic::iterator::basic{
      --bp.end(), iuic::iterator::mode::reverse,
      iuic::iterator::extension::iterator<iuic::iterator::basic>{}});

  layout::element_access_iterator ait{bp.begin()};

  for (auto el : range) {
    ait = el;

    std::visit(
        [&](auto obj) {
          if constexpr (std::same_as<decltype(obj), const layout::frame *>) {

            if (auto res = obj->measure(layout::measure::frame_utils{
                    tenv, iuic::iterator::basic{ait, el}})) {

              ait->is = ui::element::measured{res.value()};
            } else {
              ait->is = ui::element::discarded{};
              std::println("measure discard");
            };
          } else if constexpr (std::same_as<decltype(obj),
                                            const layout::text *>) {
            if (auto res = obj->measure(layout::measure::text_utils{
                    tenv, ait->text, iuic::iterator::basic{ait, el}})) {
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

void arrange_root(environment::tmp &tenv, layout::tree &bp) {
  iuic::units::ui::position cursor{units::pixel{0}, units::pixel{0}};
  iuic::units::upixel margin{0};

  auto range = iuic::iterator::range::from(tree::childs_of(bp.root()));

  for (auto &el : range) {
    // DO JOB

    if (not std::holds_alternative<ui::element::measured>(el.is)) {
      // TODO: normal handling
      continue;
    }

    auto area = calc_root_child_area(
        std::get<ui::element::measured>(el.is), tenv.style.get(el.sid),
        tenv.meta.viewport_size, tenv.meta.segment_size);

    area.bordered.y = area.bordered.y + cursor.y;
    area.borderless.y = area.borderless.y + cursor.y;

    // TODO : calc margin
    auto &is = el.is;
    is = ui::element::placed{area};

    cursor.y = cursor.y + area.bordered.h;
  };
  std::get<ui::element::placed>(iterator::access{bp.begin()}->is);
};

void arrange(environment::tmp &tenv, layout::tree &bp) {
  arrange_root(tenv, bp);

  auto range = iuic::iterator::range::from(iuic::iterator::basic{
      bp.begin(), iuic::iterator::mode::liner,
      iuic::iterator::extension::iterator<iuic::iterator::basic>{}});

  for (auto el : range) {
    iterator::access ait{el};

    // TODO : DISCARTED | VIRTUALIZED
    if (not std::holds_alternative<ui::element::placed>(ait->is)) {
      if (std::holds_alternative<ui::element::virtualized>(ait->is)) {
        // do virtualize
      } else {
        iterator::access{el}->is = ui::element::discarded{};

        for (auto ch : iuic::iterator::range::from(tree::childs_of(el))) {
          ch.is = ui::element::discarded{};
        };
      }
      continue;
    }

    std::visit(
        [&](auto obj) {
          if constexpr (std::same_as<decltype(obj), const layout::frame *>) {
            if (iuic::iterator::basic rit{ait, iuic::iterator::mode::root};
                ++rit) {
              ait->clip = iuic::iterator::access{rit}->clip;
            }

            if (not obj->arrange(
                    iuic::layout::arrange::frame_utils{tenv, el})) {
              ait->is = ui::element::discarded{};
              return;
            };
          } else {
            if (auto res = obj->arrange(
                    iuic::layout::arrange::text_utils{tenv, ait->text, el})) {
              tenv.text.get(ait->text).value = res.value();
            } else {
              ait->is = ui::element::discarded{};
              tenv.text.get(ait->text).value = text::data::corrupted{};
            }
          }
        },
        ait->type);
  };
};

}; // namespace

namespace iuic::scheme {

bool compute(layout::tree &bp, environment::tmp &tenv,
             environment::persist &penv) {

  bp.organize(iuic::tree::order::levelorder);

  measure(tenv, bp);
  arrange(tenv, bp);

  return true;
};
}; // namespace iuic::scheme
