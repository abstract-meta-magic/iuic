
module;

#include <memory>
#include <print>
#include <string_view>
#include <variant>
#include <vector>

module iuic.core;
import :layout;
import :computing.kernel;

namespace iuic {

style::cref layout_utils_base::self_style() const {
  return *kernel.get_style(element).value();
};

style::cref layout_utils_base::parent_style() const {
  if (auto s = kernel.get_style(kernel.get_parent(element))) {
    return *s.value();
  } else {
    return {nullptr};
  }
};

style::cref layout_utils_base::root_style() const {
  if (auto s = kernel.get_style({computing::element::root})) {
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
  return std::get<upixel_t>(root_style().get_shape().max_size.h) * vh;
}

upixel_t layout_utils_base::vw(vw_t vw) const noexcept {
  return std::get<upixel_t>(root_style().get_shape().max_size.w) * vw;
}

// TODO : log info
void layout_utils_base::error(std::string_view message) const noexcept {}
void layout_utils_base::log(std::string_view message) const noexcept {}
void layout_utils_base::warning(std::string_view message) const noexcept {}

// AREA
frame_measure_utils::frame_measure_utils(computing::kernel_hardware &kernel_,
                                         computing::element element_) noexcept
    : layout_utils_base{kernel_, element_} {}

// ARRANGE

std::unique_ptr<virtual_iterator<const computing::request>>
frame_arrange_utils::get_requests() {
  return kernel.get_requests(element);
}

ui_rect frame_arrange_utils::self_area() const {
  auto area = kernel.get_rect_bordered(element);
  if (area) {
    return area.value();
  } else {
    throw computing::violated_order{};
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
    auto mem = static_cast<text::glyph::placement *>(
        tmp_resource.allocate(sizeof(text::glyph::placement) * sq.size(),
                              alignof(text::glyph::placement)));

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
