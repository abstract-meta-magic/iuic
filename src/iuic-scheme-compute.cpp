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
               units::ui::size viewport) {
  return std::visit(
      [&]<typename type>(type &obj) -> units::pixel {
        if constexpr (std::same_as<type, units::percent>) {
          return static_cast<units::upixel>(psize * obj);
        } else if constexpr (std::same_as<type, units::vh>) {
          return static_cast<units::upixel>(viewport.h * obj);
        } else if constexpr (std::same_as<type, units::vw>) {
          return static_cast<units::upixel>(viewport.w * obj);
        } else if constexpr (std::same_as<type, units::upixel>) {
          return obj;
        }
        return 0;
      },
      value);
};

static constexpr inline units::ui::area
calc_root_child_area(const layout::measure::result &m, style::value style,
                     units::ui::size viewport) {

  auto width = adapt_to_pixel(m.width, viewport.w, viewport);
  auto height = adapt_to_pixel(m.width, viewport.h, viewport);

  // border
  auto &shape = style.get_shape();

  auto border_left = adapt_to_pixel(shape.border.left, viewport.w, viewport);
  auto border_right = adapt_to_pixel(shape.border.right, viewport.w, viewport);
  auto border_top = adapt_to_pixel(shape.border.top, viewport.h, viewport);
  auto border_bottom =
      adapt_to_pixel(shape.border.bottom, viewport.h, viewport);

  // margin
  auto margin_left = adapt_to_pixel(shape.margin.left, viewport.w, viewport);
  auto margin_right = adapt_to_pixel(shape.margin.right, viewport.w, viewport);
  auto margin_top = adapt_to_pixel(shape.margin.top, viewport.h, viewport);
  auto margin_bottom =
      adapt_to_pixel(shape.margin.bottom, viewport.h, viewport);

  return {};
};
}; // namespace

namespace iuic::scheme {
blueprint compute(sketch &sketch, environment::persist &penv) {
  //

  blueprint b{
      &sketch.tenv, &penv,
      sketch.tree.reflect([](const sketch::element &el) -> blueprint::element {
        return {.uid = el.uid, .sid = el.sid, .zorder = el.zorder};
      })};

  layout::measure::tree m{sketch.tree.reflect(
      [](const sketch::element &el) -> layout::measure::unit {
        return {.sid = el.sid};
      })};

  std::println("---beg");
  // need :
  // - reverse bfs
  // - call layout::measure
  {
    auto range = utils::tree::reverse_bfs_range_for{sketch.tree};

    for (auto [cur, end] = range.range(); cur != end; ++cur) {
      //
      utils::tree::access_iterator ait{cur};
      auto bit = utils::tree::shift(b.tree.begin(), cur);
      auto chit = utils::tree::childs_of(utils::tree::shift(m.begin(), cur));

      std::visit(
          [&](auto obj) {
            using type = std::remove_cvref_t<decltype(obj)>;

            if constexpr (std::same_as<type, const layout::frame *>) {
              auto res = obj->measure(
                  layout::measure::frame_utils{sketch.tenv, bit, chit});

              auto mit = utils::tree::shift(m.begin(), cur);

              if (res) {
                (utils::tree::access_iterator{mit})->measure =
                    std::move(res.value());
              } else {
                utils::tree::access_iterator ait{bit};

                // make all ch discarted
                ait->meta.set(ait->meta.discarded);
                return;
              }

            } else if constexpr (std::same_as<type, const layout::text *>) {
              // obj->measure(layout::measure::text_utils{});
            }

            utils::tree::access_iterator ait{bit};
            ait->meta.set(ait->meta.measure);
          },
          ait->layout);
    };
  }

  // need :
  // - bfs
  // - call layout::arrange
  {

    auto root_ch = utils::tree::childs_of(sketch.tree.root());

    // apply root childs
    for (utils::tree::sentinel end{root_ch}; root_ch != end; ++root_ch) {
      units::ui::position pos{0, 0};

      auto bit = utils::tree::shift(b.tree.begin(), root_ch);
      utils::tree::access_iterator ait{bit};

      utils::tree::access_iterator mesure_acc{
          utils::tree::shift(m.begin(), root_ch)};

      auto viewport = penv.external.get_viewport_size();

      // make area
      auto area = calc_root_child_area(mesure_acc->measure,
                                       sketch.tenv.style.get(mesure_acc->sid),
                                       viewport);
      // set position

      ait->meta.set(ait->meta.applied);
    }

    auto range = utils::tree::bfs_range_for{sketch.tree};

    for (auto [cur, end] = range.range(); cur != end; ++cur) {

      utils::tree::access_iterator ait{cur};
      auto bit = utils::tree::shift(b.tree.begin(), cur);

      if (auto ait = utils::tree::access_iterator{bit};
          ait->meta.has(ait->meta.discarded)) {
        continue;
      } else if (auto pait =
                     utils::tree::access_iterator{utils::tree::parent_of(ait)};
                 pait && (pait->meta.has(pait->meta.discarded) ||
                          not ait->meta.has(ait->meta.applied))) {
        ait->meta.set(ait->meta.discarded);
        continue;
      }

      auto chit = utils::tree::childs_of(utils::tree::shift(m.begin(), cur));

      std::visit(
          [&](auto obj) {
            using type = std::remove_cvref_t<decltype(obj)>;

            if constexpr (std::same_as<type, const layout::frame *>) {
              if (not obj->arrange(
                      layout::arrange::frame_utils{sketch.tenv, bit, chit})) {
                utils::tree::access_iterator ait{bit};
                // discard all childs
                ait->meta.set(ait->meta.discarded);
              };
            } else if constexpr (std::same_as<type, const layout::text *>) {
              // obj->arrange(layout::arrange::text_utils{});
            };

            utils::tree::access_iterator ait{bit};
            ait->meta.set(ait->meta.arrange);
          },
          ait->layout);
    }
  }

  return b;
};
}; // namespace iuic::scheme
