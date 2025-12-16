

module;

export module iuic.core:layout.text.box;
import :layout;

namespace iuic {

struct text_def_layout : text_layout {

  measure_result measure(text_measure_utils utils) const noexcept override {
    std::println("???");
    return {{0, 0}};
  };

  text::glyph::sequence
  arrange(text_arrange_utils utils) const noexcept override {
    return {};
  };

} constexpr text_def_layout{};
}; // namespace iuic
