
module;
#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <iterator>
#include <print>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

module iuic.core;
import :hash;

namespace iuic {

void context::set_view_size(ui_size size) { ctree.set_root_size(size); };

const std::vector<relement> &context::get_tree() { return to_render; }

void context::reset() {
  ctree.reset();
  to_render.clear();
  object.advance_generation();
  text.advance_generation();
};

void context::proccess_measure() {

  // warning

  auto rtree = ctree.reverse_range_for();

  for (auto &&cc : ctree.reverse_range_for()) {
    if (cc.is_discarted()) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            auto res = layout->measure({&cc});

            // TODO : fix me
            if (res) {
              cc.apply(res.value().rq); // this wrong. no check measure result
            } else {
              cc.discard();
            }
          } else if constexpr (std::same_as<decltype(layout),
                                            const text_layout *>) {
            if (auto tp = tpa.get_present(
                    cc.get_hierarchy().interface->get_id(&cc))) {
              if (auto buff = text.get(tp->second)) {
                if (auto res = layout->measure({&cc, buff})) {
                  cc.apply(res.value().rq);
                } else {
                  cc.discard();
                };
              }
            }
          }
        },
        cc.get_layout());
  }

  // mda root calc
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          auto res = layout->measure({&ctree.root()});

          if (res) {
            ctree.root().apply(res.value().rq);
          } else {
            ctree.root().discard();
          };
        }
      },
      ctree.root().get_layout());
};

void context::proccess_position() {
  // TODO : PARALLEL
  ctree.root().apply(ui_position{0, 0});
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->position({&ctree.root()});
        }
      },
      ctree.root().get_layout());

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            layout->position({&cc});
          }
        },
        cc.get_layout());
  }
}

void context::proccess_arrange() {
  // TODO : PARALLEL
  auto &max_size = ctree.root().get_info().style->shape.max_size;
  ctree.root().apply(
      ui_size{std::get<upixel_t>(max_size.w), std::get<upixel_t>(max_size.h)});

  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->arrange({&ctree.root()});
        }
      },
      ctree.root().get_layout());

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            if (not layout->arrange({&cc})) {
              cc.discard();
            }
          } else if constexpr (std::same_as<decltype(layout),
                                            const text_layout *>) {
            if (auto tp = tpa.get_present(
                    cc.get_hierarchy().interface->get_id(&cc))) {
              auto &text_present = tp->first;

              // can discart
              if (layout->arrange({&cc, &tp->first})) {
                if (auto asize = cc.get_size()) {
                  text_present.rect.size = asize.value();
                }
                cc.apply(&text_present);
              } else {
                cc.discard();
              }
            }
          }
        },
        cc.get_layout());
  }
};

void context::build_render_list() {
  // тут нужно будет отсеять лишние и
  // маркерованные элементы
  // после чего составить простой список
  // отрисовки

  for (auto &cc : ctree.range_for()) {
    auto rect = cc.get_rect();
    if (not rect) {
      // err ?
      continue;
    }

    to_render.push_back({.area{rect.value()},
                         .data{frame_render_data{
                             .background{cc.get_info().style->background}}}});
  }
}
}; // namespace iuic
