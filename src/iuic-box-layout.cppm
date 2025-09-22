
module;
#include <concepts>
#include <expected>
#include <iostream>
#include <print>
#include <span>
#include <type_traits>
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

    ui_size vmargin{0};
    upixel_t max_height{0};
    upixel_t max_width{0};

    auto requests = utils.get_requests();

    for (auto &rq : requests) {
      auto value = rq.value();
      auto &margin = rq.style_of().positioning.margin;

      if (auto h = utils.upixel_of(value.height)) {
        max_height += h.value();
      }

      if (auto top = utils.upixel_of(margin.top)) {
        max_height += top.value();
      }

      auto margin_left = utils.upixel_of(margin.left);
      auto margin_right = utils.upixel_of(margin.right);

      upixel_t horisontal_size = (margin_left ? margin_left.value() : 0) +
                                 (margin_right ? margin_right.value() : 0);

      if (auto width = utils.upixel_of(value.width)) {
        horisontal_size += width.value();
      }

      if (horisontal_size > max_width) {
        max_width = horisontal_size;
      }

      std::println("chaild : w:{},h:{}", max_width, max_height);
      // do
    }

    measure_request res{};

    auto &min = utils.self_style().shape.min_size;

    if (auto min_h = utils.upixel_of(min.h)) {
      if (min_h.value() > max_height) {
        res.rq.height = min_h.value();
      } else {
        res.rq.height = max_height;
      }
    } else {
      res.rq.height = max_height;
    };

    if (auto min_w = utils.upixel_of(min.w)) {
      if (min_w.value() > max_width) {
        res.rq.width = min_w.value();
      } else {
        res.rq.width = max_width;
      }
    } else {
      res.rq.width = max_width;
    }

    return res;
  };

  void arrange(frame_arrange_utils utils) const noexcept override {
    auto requests = utils.get_requests();

    auto self_size = utils.self_size();

    for (auto &rq : requests) {
      auto value = rq.value();
      auto &min = rq.style_of().shape.min_size;

      ui_size res = {.w = utils.width_upixel_of(value.width),
                     .h = utils.height_upixel_of(value.height)};

      auto style_min_width = utils.width_upixel_of(min.w);
      if (style_min_width > res.w) {
        res.w = style_min_width;
      }

      auto style_min_height = utils.height_upixel_of(min.h);
      if (style_min_height > res.h) {
        res.h = style_min_height;
      }

      std::println("apply : w:{},h:{}", res.w, res.h);

      rq.apply(res);
    };
  };

  static upixel_t margin_top(frame_position_utils &utils,
                             position_request &rq) noexcept {
    return std::visit(
        [&](auto &margin_top) -> upixel_t {
          using type = std::remove_cvref_t<decltype(margin_top)>;
          if constexpr (std::same_as<type, upixel_t>) {
            return margin_top;
          } else if constexpr (std::same_as<type, percent_t>) {
            return utils.self_size().h * margin_top;
          } else if constexpr (std::same_as<type, vw_t>) {
            return utils.vw(margin_top);
          } else if constexpr (std::same_as<type, vh_t>) {
            return utils.vh(margin_top);
          } else if constexpr (std::same_as<type, rem_t>) {
            return utils.rem(margin_top);
          } else {
            return {};
          }
        },
        rq.style_of().positioning.margin.top);
  };

  static upixel_t margin_left(frame_position_utils &utils,
                              position_request &rq) noexcept {
    return std::visit(
        [&](auto &margin_left) -> upixel_t {
          using type = std::remove_cvref_t<decltype(margin_left)>;
          if constexpr (std::same_as<type, upixel_t>) {
            return margin_left;
          } else if constexpr (std::same_as<type, percent_t>) {
            return utils.self_size().w * margin_left;
          } else if constexpr (std::same_as<type, vw_t>) {
            return utils.vw(margin_left);
          } else if constexpr (std::same_as<type, vh_t>) {
            return utils.vh(margin_left);
          } else if constexpr (std::same_as<type, rem_t>) {
            return utils.rem(margin_left);
          } else {
            return {};
          }
        },
        rq.style_of().positioning.margin.left);
  };

  void position(frame_position_utils utils) const noexcept override {
    auto position = utils.self_position();
    auto def = position;

    auto content = utils.content();

    for (auto &&rq : content) {
      auto &style = rq.style_of();

      position.y += margin_top(utils, rq);
      position.x += margin_left(utils, rq);
      auto size = rq.size_of();
      rq.apply(position);
      position.y += size.h;
      position.x = def.x;
    }
  };
} constexpr inline box_layout{};
}; // namespace iuic
