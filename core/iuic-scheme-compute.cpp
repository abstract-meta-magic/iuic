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

void measure(environment::tmp &tenv, scheme::sketch &sketch,
             layout::tree_type &tree) {

  auto range = tree::iterator_range_for{
      sketch, tree::iterator_type<tree::base_iterator>{},
      tree::tag::reverse_levelorder{}};

  for (auto el : range) {
    auto sk_ait = tree::access_iterator{el};
    auto tree_ait = tree::access_iterator{tree::shift(tree.begin(), el)};

    std::visit(
        [&](auto obj) {
          if constexpr (std::same_as<decltype(obj), const layout::frame *>) {

            if (auto res = obj->measure(layout::measure::frame_utils{
                    tenv, tree::base_iterator{tree_ait}})) {
              tree_ait->measure = res.value();
            } else {
              tree_ait->meta.set_discarted();
              return;
            };
          } else {
            if (auto res = obj->measure(layout::measure::text_utils{
                    tenv, sk_ait->text, tree::base_iterator{tree_ait}})) {
              tree_ait->measure = res.value();
              tree_ait->meta.set_text_mark();
            } else {
              tree_ait->meta.set_discarted();
              return;
            };
          };
          tree_ait->meta.set_measured();
        },
        sk_ait->layout);
  };
};

void arrange_root(environment::tmp &tenv, scheme::sketch &sketch,
                  layout::tree_type &tree) {
  auto root_ch = tree::childs_of(sketch.root());

  iuic::units::ui::position cursor{units::pixel{0}, units::pixel{0}};
  iuic::units::upixel margin{0};

  for (auto el : tree::iterator_range_for{
           root_ch, tree::iterator_type<tree::base_iterator>{}}) {
    // DO JOB
    auto sk_ait = tree::access_iterator{el};
    auto tree_ait = tree::access_iterator{tree::shift(tree.begin(), el)};

    auto area =
        calc_root_child_area(tree_ait->measure, tree_ait->style,
                             tenv.meta.viewport_size, tenv.meta.segment_size);

    area.bordered.y = area.bordered.y + cursor.y;
    area.borderless.y = area.borderless.y + cursor.y;

    tree_ait->arrange = area;
    tree_ait->meta.set_applyed();

    cursor.y = cursor.y + area.bordered.h;
  };
};

void arrange(environment::tmp &tenv, scheme::sketch &sketch,
             layout::tree_type &tree) {

  arrange_root(tenv, sketch, tree);

  for (auto el : tree::iterator_range_for{
           sketch, tree::iterator_type<tree::base_iterator>{}}) {
    auto sk_ait = tree::access_iterator{el};
    auto tree_ait = tree::access_iterator{tree::shift(tree.begin(), el)};

    if (tree_ait->meta.is_discarted() || not tree_ait->meta.is_applyed()) {
      for (auto ch : tree::iterator_range_for{
               tree::childs_of(tree_ait),
               tree::iterator_type<tree::base_iterator>{}}) {
        tree::access_iterator{ch}->meta.set_discarted();
      }
      continue;
    }

    std::visit(
        [&](auto obj) {
          if constexpr (std::same_as<decltype(obj), const layout::frame *>) {
            // frame
            if (not obj->arrange(iuic::layout::arrange::frame_utils{
                    tenv, tree::base_iterator{tree_ait}})) {
              tree::access_iterator{tree::shift(tree.begin(), el)}
                  ->meta.set_discarted();
              return;
            };
          } else {
            auto res = obj->arrange(iuic::layout::arrange::text_utils{
                tenv, sk_ait->text, tree::base_iterator{tree_ait}});
            tree_ait->text = res; // check empty ???
          }

          tree_ait->meta.set_arranged();
        },
        tree::access_iterator{el}->layout);
  };
};

}; // namespace

namespace iuic::scheme {

blueprint compute(sketch &sketch, environment::tmp &tenv,
                  environment::persist &penv) {
  layout::tree_type layout_tree{
      sketch.reflect([&](const sketch::value_t &el) -> layout::element {
        return {.style = tenv.style.get(el.sid)};
      })};

  measure(tenv, sketch, layout_tree);
  arrange(tenv, sketch, layout_tree);

  // resolve
  auto b = sketch.reflect([](sk_element sk) -> bp_element {
    return {.uid = sk.uid, .sid = sk.sid, .zorder = sk.zorder};
  });

  for (auto el : tree::iterator_range_for{
           layout_tree, tree::iterator_type<tree::base_iterator>{}}) {
    tree::access_iterator{tree::shift(b.begin(), el)}->area =
        tree::access_iterator{el}->arrange;

    tree::access_iterator{tree::shift(b.begin(), el)}->text =
        tree::access_iterator{el}->text;
  };

  return b;
};
}; // namespace iuic::scheme
