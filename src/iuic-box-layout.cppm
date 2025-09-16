
module;
#include <expected>
#include <iostream>
#include <print>
#include <span>
#include <variant>
#include <vector>

export module iuic.core:layout.box;
import :layout.def;
export import :layout;

namespace iuic {
// базовый приватный layout для всех
// если в стиле отсутствует layout
// для вычислений используется этот
struct final : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override {
    auto &style = utils.self_style();

    measure_request res{};

    ui_size vmargin{0};
    ui_size hieght{0};
    ui_size max_width{0};

    auto requests = utils.get_requests();

    return {{percent_t{20}, percent_t{20}}};
  };

  void arrange(frame_arrange_utils utils) const noexcept override {
    auto requests = utils.get_requests();

    auto self_size = utils.get_size();

    for (auto &rq : requests) {
      auto value = rq.value();

      rq.apply({200, 100});
    };
  };

  void position(frame_position_utils utils) const noexcept override {
    auto position = utils.self_position();
    auto def = position;

    auto content = utils.content();

    for (auto &&rq : content) {
      auto &style = rq.style_of();

      position.y += style.positioning.margin.top;
      position.x += style.positioning.margin.left;
      auto size = rq.size_of();
      rq.apply(position);
      position.y += size.h;
      position.x = def.x;
    }
  };
} constexpr inline box_layout{};
}; // namespace iuic
