module;

module iuic.core;
import std;
import :hash;

namespace iuic {

void context::set_view_size(ui_size size) {
  // TODO
  // to static_cast
  auto *mem = static_cast<style::shape *>(
      kernel->memory()->tmp(kernel::memory_model::type::from<style::shape>()));

  if (mem) {
    new (mem) style::shape{};

    mem->max_size.w = upixel_t{size.w};
    mem->max_size.h = upixel_t{size.h};
    kernel->override({.meta = kernel::element::root}, mem);
  }
};

// Refactor and move to other file
template <> void advance(scheme::builder &builder) {
  builder.unit.top().index = 0;
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
void context::reset() { kernel->advance(); };

void context::proccess_measure() {

  int coutnt{0};
  for (auto &el : kernel->get_elements(true)->range()) {
    auto te = el.self;

    if (el.meta && kernel::element::discarded) {
      continue;
    }

    std::visit(
        [&](auto layout) {
          if constexpr (std::same_as<decltype(layout), const frame_layout *>) {
            if (not layout) {
              kernel->discard(el);
              return;
            }

            auto res = layout->measure({*kernel, el});

            // TODO : fix me
            if (res) {
              kernel->attach(el, res.value().rq);
            } else {
              kernel->discard(el);
            }
          } else if constexpr (std::same_as<decltype(layout),
                                            const text_layout *>) {

            auto tqs = tpa.get_tokens(el.self);

            auto res = layout->measure({*kernel, el, tqs, font});
            if (res) {
              kernel->attach(el, res.value().rq);
            } else {
              kernel->discard(el);
            }
          }
        },
        kernel->get_layout(el));
  }

  // mda root calc
  kernel::element root{.meta = kernel::element::root};
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

  kernel::element root{.meta = kernel::element::root};

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
    if (el.meta && kernel::element::discarded) {
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
    if (el.meta & kernel::element::arrange) {
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
