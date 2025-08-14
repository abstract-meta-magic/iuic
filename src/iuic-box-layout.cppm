
module;
#include <iostream>
#include <print>
#include <span>

export module iuic.core:layout.box;
import :base;

namespace iuic {
// базовый приватный layout для всех
// если в стиле отсутствует layout
// для вычислений используется этот
struct box_layout final : public layout {
  void self_size(area_utils utils) const noexcept override {

    ui_size res{0, 0};

    auto &style = utils.self_style();

    std::println("My size : H-{},W-{}", style.shape.min_size.h,
                 style.shape.min_size.w);

    auto requests = utils.get_requests();

    for (auto &&rq : requests) {

      std::println("Request area size - h:{},w:{}", rq.value().h, rq.value().w);

      res.h += rq.style_of().shape.margin.top;
      res.h += rq.value().h;

      if (style.shape.max_size.h != 0 && res.h > style.shape.max_size.h) {
        break;
      }

      upixel_t w = rq.value().w + rq.style_of().shape.margin.left;
      if (w > res.w) {
        res.w = w;
      }

      rq.apply();
    }

    auto rq = minmax(utils.self_style(), res);

    std::println("My area request - h:{},w:{}", rq.h, rq.w);
    utils.request_size(rq);

    // мб сделать в стиле std::prindln ?
    utils.log("box - complite");
  };

  void set_childs_position(position_utils utils) const noexcept override {
    auto position = utils.self_position();
    auto def = position;

    auto content = utils.content();

    std::println("Start element position : x-{},y-{}", position.x, position.y);

    for (auto &&rq : content) {
      auto &style = rq.style_of();

      position.y += style.shape.margin.top;
      position.x += style.shape.margin.left;
      rq.apply(position);
      auto &size = rq.size_of();
      position.y += size.h;
      position.x = def.x;
      std::println("Child request size : h-{},w-{}", size.h, size.w);
    }
  };

  void balancing(balancing_utils utils) const noexcept override {
    if (utils.is_strong_applied()) {
      return;
    }
  };
};
}; // namespace iuic
