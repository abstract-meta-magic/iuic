
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
#include <variant>
#include <vector>

module iuic.core;
import :hash;

namespace iuic {

void context::set_view_size(ui_size size) { ctree.set_root_size(size); };

// Refactor and move to other file
template <> void advance(context::builder &builder) {
  builder.uids[0].index = 0;
};

// Refactor and move to other file
template <> void advance(managed_object_storage &storage) {
  storage.advance_generation();
};

// Refactor and move to other file
template <> void advance(managed_text_storage &storage) {
  storage.advance_generation();
};

// Refactor and move to other file
template <> void advance(computing::tree &ctree) { ctree.reset(); };

// TODO : replace all to advance
void context::reset() {
  iuic::advance(ctree);
  iuic::advance(object);
  iuic::advance(text);
  frame_resource__.release();
  iuic::advance(b);
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

            auto res = layout->measure(
                {&cc, tpa.get_linked_text(
                          cc.get_hierarchy().interface->get_id(&cc))});
            if (res) {
              cc.apply(res.value().rq);
            } else {
              cc.discard();
            }
            // TODO compute
          }
        },
        cc.get_layout());
  }

  // mda root calc
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          auto res = layout->measure({ctree.root()});

          if (res) {
            ctree.root()->apply(res.value().rq);
          } else {
            ctree.root()->discard();
          };
        }
      },
      ctree.root()->get_layout());
};

void context::proccess_position() {
  // TODO : PARALLEL
  ctree.root()->apply(ui_position{0, 0});
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->position({ctree.root()});
        }
      },
      ctree.root()->get_layout());

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
  auto &max_size = ctree.root()->get_info().style.get_shape().max_size;
  ctree.root()->apply(
      ui_size{std::get<upixel_t>(max_size.w), std::get<upixel_t>(max_size.h)});

  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->arrange({ctree.root()});
        }
      },
      ctree.root()->get_layout());

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
            auto id = cc.get_hierarchy().interface->get_id(&cc);

            // apply rect ?
            text::present present;

            if (layout->arrange({&cc, present, tpa.get_linked_text(id)})) {
              tpa.apply_present(id, std::move(present));
            } else {
              cc.discard();
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

  scheme::incomplete inc;

  for (auto &cc : ctree.range_for()) {
    if (not cc.is_discarted()) {
      if (std::holds_alternative<const frame_layout *>(cc.get_layout())) {
        inc.push_frame(cc.get_rect().value(), cc.get_info().style);
      } else {
        inc.push_text(
            cc.get_rect().value(),
            &tpa.get_present(cc.get_hierarchy().interface->get_id(&cc)),
            cc.get_info().style);
      }
    }
  }

  scheme = std::move(inc);
}
}; // namespace iuic
