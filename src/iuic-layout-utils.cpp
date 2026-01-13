
module;

module iuic.core;
import std;
import :layout;
import :kernel;

namespace iuic {

style::cref layout_utils_base::self_style() const {
  return *kernel.get_style(element).value();
};

style::cref layout_utils_base::style_of(const kernel::request &rq) const {
  return *kernel.get_style(rq.element).value();
};

void layout_utils_base::discard(const kernel::request &rq) {
  kernel.discard(rq.element);
};

style::cref layout_utils_base::parent_style() const {
  if (auto s = kernel.get_style(kernel.get_parent(element))) {
    return *s.value();
  } else {
    return {nullptr};
  }
};

style::cref layout_utils_base::root_style() const {
  if (auto s = kernel.get_style({.meta = kernel::element::root})) {
    return *s.value();
  }

  return {nullptr};
};

ui_size layout_utils_base::root_size() const {
  return {std::get<upixel_t>(root_style().get_shape().max_size.w),
          std::get<upixel_t>(root_style().get_shape().max_size.h)};
};

upixel_t layout_utils_base::rem(rem_t rem) const noexcept {
  return root_style().get_advance().ephemeral_value * rem;
};

upixel_t layout_utils_base::vh(vh_t vh) const noexcept {
  auto h = std::get<upixel_t>(root_style().get_shape().max_size.h) * vh;
  return h;
}

upixel_t layout_utils_base::vw(vw_t vw) const noexcept {
  auto w = std::get<upixel_t>(root_style().get_shape().max_size.w) * vw;
  return w;
}

// TODO : log info
void layout_utils_base::error(std::string_view message) const noexcept {}
void layout_utils_base::log(std::string_view message) const noexcept {}
void layout_utils_base::warning(std::string_view message) const noexcept {}

// AREA
frame_measure_utils::frame_measure_utils(kernel::hardware &kernel_,
                                         kernel::element element_) noexcept
    : layout_utils_base{kernel_, element_} {}
std::unique_ptr<virtual_iterator<const kernel::request>>
frame_measure_utils::get_requests() {
  return kernel.get_requests(element);
}

// ARRANGE

std::unique_ptr<virtual_iterator<const kernel::request>>
frame_arrange_utils::get_requests() {
  return kernel.get_requests(element);
}

void frame_arrange_utils::apply(const kernel::request &rq, ui_rect rect) {
  kernel.apply(rq.element, rect);
}

void frame_arrange_utils::apply(const kernel::request &rq, ui_rect brect,
                                ui_rect rect) {
  kernel.apply(rq.element, brect, rect);
}

ui_rect frame_arrange_utils::self_area() const {
  auto area = kernel.get_rect_bordered(element);
  if (area) {
    return area.value();
  } else {
    throw kernel::violated_order{};
  };
};

text::token::sequence text_measure_utils::get_tokens() const { return sq; };

const text::glyph::atlas &text_measure_utils::get_atlas(style::font::cref ref) {
  return font.get(ref);
};

text::token::sequence text_arrange_utils::get_tokens() const { return sq; };

const text::glyph::atlas &text_arrange_utils::get_atlas(style::font::cref ref) {
  return font.get(ref);
};

text::glyph::sequence
text_arrange_utils::capture_glyphs(text::glyph::sequence sq) const {

  if (sq.empty()) {
    return {};
  } else {
    auto mem = static_cast<text::glyph::placement *>(kernel.memory()->tmp(
        erasure::type::from<text::glyph::placement>(), sq.size()));

    if (mem) {
      auto current = mem;

      for (auto &&glyph : sq) {
        new (current) text::glyph::placement{glyph};
        ++current;
      }

      return {mem, current};
    }
  }
  return {};
};

ui_rect text_arrange_utils::self_rect() const noexcept {
  auto area = kernel.get_rect_bordered(element);
  if (area) {
    return area.value();
  }

  return {0, 0};
};
}; // namespace iuic
