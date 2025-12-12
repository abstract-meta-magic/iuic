
module;
#include <cassert>
#include <cstddef>
#include <cstdlib>
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

void context::set_view_size(ui_size size) {
  // TODO
};

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

// TODO : replace all to advance
void context::reset() { iuic::advance(b); };

void context::proccess_measure() {

  for (auto &&el : kernel->get_elements(true)->range()) {
    if (el.meta & computing::element::discarded) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            auto res = layout->measure({*kernel, el});

            // TODO : fix me
            if (res) {
              kernel->attach(el, res.value().rq);
            } else {
              kernel->discard(el);
            }
          } else if constexpr (std::same_as<decltype(layout),
                                            const text_layout *>) {

            auto res =
                layout->measure({*kernel, el, tpa.get_tokens(el.self), font});
            if (res) {
              kernel->attach(el, res.value().rq);
            } else {
              kernel->discard(el);
            }
            // TODO compute
          }
        },
        kernel->get_layout(el));
  }

  // mda root calc
  computing::element root{computing::element::root};
  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          auto res = layout->measure({*kernel, root});

          if (res) {
            kernel->attach(root, res.value().rq);
          } else {
            kernel->discard(root);
          };
        }
      },
      kernel->get_layout(root));
};

void context::proccess_arrange() {
  // TODO : PARALLEL

  computing::element root{computing::element::root};

  auto &max_size = kernel->get_style(root).value()->get_shape().max_size;

  kernel->apply(root, ui_rect{0, 0, std::get<upixel_t>(max_size.w),
                              std::get<upixel_t>(max_size.h)});

  std::visit(
      [&](auto layout) {
        if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
          layout->arrange({*kernel, root});
        }
      },
      kernel->get_layout(root));

  for (auto &el : kernel->get_elements()->range()) {
    if (el.meta & computing::element::discarded) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            if (not layout->arrange({*kernel, el})) {
              kernel->discard(el);
            }
          }
        },
        kernel->get_layout(el));
  }
};

void context::build_render_list() {
  // тут нужно будет отсеять лишние и
  // маркерованные элементы
  // после чего составить простой список
  // отрисовки

  scheme::incomplete inc;

  for (auto &el : kernel->get_elements()->range()) {
    if (el.meta & computing::element::arrange) {
      if (std::holds_alternative<const frame_layout *>(
              kernel->get_layout(el))) {
        inc.push_frame(kernel->get_rect_bordered(el).value(),
                       *kernel->get_style(el).value());
      } else {
        inc.push_text(kernel->get_rect_bordered(el).value(),
                      tpa.get_present(el.self), *kernel->get_style(el).value());
      }
    }
  }

  scheme = std::move(inc);
}
}; // namespace iuic
