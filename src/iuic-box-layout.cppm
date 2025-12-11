
module;
#include <concepts>
#include <expected>
#include <iostream>
#include <print>
#include <span>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:layout.frame.box;
import :layout.def;
export import :layout;

namespace iuic {

struct : frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override {

    return {{0, 0}};
  };

  bool arrange(frame_arrange_utils utils) const noexcept override {
    return false;
  };
} box_layout;

}; // namespace iuic
