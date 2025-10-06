

module;

#include <print>
export module iuic.core:layout.text.box;
import :layout;

namespace iuic {

struct text_def_layout : text_layout {

  measure_result measure(text_measure_utils utils) const noexcept override {
    return {{0, 0}};
  };

  bool arrange(text_arrange_utils utils) const noexcept override {
    return false;
  };

} constexpr text_def_layout{};
}; // namespace iuic
