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
calc_root_child_area(const layout::measure::result &m, style::value style,
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
}; // namespace

namespace iuic::scheme {

blueprint compute(sketch &sketch, environment::tmp &tenv,
                  environment::persist &penv) {

  blueprint blueprint{
      sketch.reflect([](const sketch::value_t &el) -> blueprint::value_t {
        std::println("reflect sid : {}", (unsigned)el.sid);
        return {.uid = el.uid, .sid = el.sid, .zorder = el.zorder};
      })};

  layout::measure::tree m{
      sketch.reflect([](const sketch::value_t &el) -> layout::measure::unit {
        return {.sid = el.sid};
      })};

  // need :
  // - reverse bfs
  // - call layout::measure
  {
    // mb use iterator_range_for ???
    auto range = tree::range_for{sketch, tree::tag::reverse_levelorder{}};

    for (auto [cur, end] = range.range(); cur != end; ++cur) {
      //
      tree::access_iterator ait{cur};
      auto bit = tree::shift(blueprint.begin(), cur);
      auto chit = tree::childs_of(tree::shift(m.begin(), cur));

      std::visit(
          [&](auto obj) {
            using type = std::remove_cvref_t<decltype(obj)>;

            if constexpr (std::same_as<type, const layout::frame *>) {
              auto res =
                  obj->measure(layout::measure::frame_utils{tenv, bit, chit});

              auto mit = tree::shift(m.begin(), cur);

              if (res) {
                (tree::access_iterator{mit})->measure = std::move(res.value());
              } else {
                tree::access_iterator ait{bit};

                ait->meta.set(ait->meta.discarded);

                return;
              }

            } else if constexpr (std::same_as<type, const layout::text *>) {
              auto res = obj->measure(layout::measure::text_utils{
                  tenv, tree::access_iterator{cur}->text, bit});

              auto mit = tree::shift(m.begin(), cur);
              if (res) {
                tree::access_iterator{mit}->measure = std::move(res.value());
              } else {
                tree::access_iterator ait{bit};
                ait->meta.set(ait->meta.discarded);
                return;
              }
            }

            tree::access_iterator ait{bit};
            ait->meta.set(ait->meta.measure);
          },
          ait->layout);
    };
  }

  // need :
  // - bfs
  // - call layout::arrange
  {

    auto root_ch = tree::childs_of(sketch.root());

    units::ui::position pos{units::pixel{0}, units::pixel{0}};
    units::upixel margin{0};
    // apply root childs
    for (tree::sentinel end{root_ch}; root_ch != end; ++root_ch) {

      auto bit = tree::shift(blueprint.begin(), root_ch);
      tree::access_iterator ait{bit};

      tree::access_iterator mesure_acc{tree::shift(m.begin(), root_ch)};

      auto &viewport = tenv.meta.viewport_size;
      auto &segment = tenv.meta.segment_size;

      // make area
      auto area = calc_root_child_area(mesure_acc->measure,
                                       tenv.style.get(mesure_acc->sid),
                                       viewport, segment);
      // set position
      auto &shape = tenv.style.get(ait->sid).get_shape();

      auto tm = adapt_to_pixel(shape.margin.top, viewport.h, viewport, segment);

      pos.y = pos.y + (tm > margin ? tm : margin);

      margin =
          adapt_to_pixel(shape.margin.bottom, viewport.h, viewport, segment);

      area.bordered.x =
          area.bordered.x +
          adapt_to_pixel(shape.margin.left, viewport.w, viewport, segment);
      area.borderless.x = area.borderless.x + area.bordered.x;

      area.bordered.y = area.bordered.y + pos.y;
      area.borderless.y = area.borderless.y + pos.y;

      pos.y = pos.y + area.bordered.h;

      ait->area = area;
      ait->meta.set(ait->meta.applied);
    }

    auto range = tree::range_for{sketch};

    for (auto [cur, end] = range.range(); cur != end; ++cur) {

      tree::access_iterator cur_acc{cur};
      auto bp_acc = tree::access_iterator{tree::shift(blueprint.begin(), cur)};

      if (auto ait = tree::access_iterator{bp_acc};
          ait->meta.has(ait->meta.discarded)) {
        continue;
      } else if (auto pait = tree::access_iterator{tree::parent_of(ait)};
                 pait && (pait->meta.has(pait->meta.discarded) ||
                          not ait->meta.has(ait->meta.applied))) {
        ait->meta.set(ait->meta.discarded);
        continue;
      }

      std::visit(
          [&](auto obj) {
            using type = std::remove_cvref_t<decltype(obj)>;

            if constexpr (std::same_as<type, const layout::frame *>) {
              layout::arrange::frame_utils utils{
                  tenv, bp_acc,
                  iuic::tree::childs_of(iuic::tree::shift(m.begin(), bp_acc))};

              if (not obj->arrange(utils)) {
                tree::access_iterator ait{bp_acc};
                ait->meta.set(ait->meta.discarded);
              };
            } else if constexpr (std::same_as<type, const layout::text *>) {
              auto res = obj->arrange(layout::arrange::text_utils{
                  tenv, tree::access_iterator{cur}->text, bp_acc});

              if (res.empty()) {
                tree::access_iterator ait{bp_acc};
                ait->meta.set(ait->meta.discarded);
              } else {
                tree::access_iterator{bp_acc}->text = res;
              };
            };

            tree::access_iterator ait{bp_acc};
            ait->meta.set(ait->meta.arrange);
          },
          cur_acc->layout);
    }
  }

  return blueprint;
};
}; // namespace iuic::scheme
