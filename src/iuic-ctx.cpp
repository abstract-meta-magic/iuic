
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
template <> void advance(scheme::builder &builder) {
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

  computing::kernel_hardware &kernel = ctree;

  auto elements = kernel.range_for();

  for (auto current = elements.rbegin(), end = elements.rend(); current != end;
       ++current) {
    auto el = *current;
    if (el.meta & computing::element::discarded) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            auto res = layout->measure({kernel, el});

            // TODO : fix me
            if (res) {
              kernel.apply(el, res.value().rq);
            } else {
              kernel.discard(el);
            }
          } else if constexpr (std::same_as<decltype(layout),
                                            const text_layout *>) {

            auto res = layout->measure({kernel, tpa.get_tokens(el.self), font});
            if (res) {
              kernel.apply(el, res.value().rq);
            } else {
              kernel.discard(el);
            }
            // TODO compute
          }
        },
        kernel.get_layout(el));
  }

  // mda root calc
  computing::element root{computing::element::root};
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          auto res = layout->measure({kernel, root});

          if (res) {
            kernel.apply(root, res.value().rq);
          } else {
            kernel.discard(root);
          };
        }
      },
      kernel.get_layout(root));
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
          } else if constexpr (std::same_as<decltype(layout),
                                            const text_layout *>) {
            auto id = cc.get_hierarchy().interface->get_id(&cc);

            if (auto res = layout->arrange(
                    {&cc, tpa.get_tokens(id), font, frame_resource__});
                not res.empty()) {
              tpa.apply_present(id, res);
            } else {
              cc.discard();
            }
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
            tpa.get_present(cc.get_hierarchy().interface->get_id(&cc)),
            cc.get_info().style);
      }
    }
  }

  scheme = std::move(inc);
}
}; // namespace iuic
