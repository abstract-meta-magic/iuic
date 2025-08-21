
module;
#include <iostream>
#include <print>
#include <span>
#include <vector>

export module iuic.core:layout.box;
export import :layout;

namespace iuic {
// базовый приватный layout для всех
// если в стиле отсутствует layout
// для вычислений используется этот
struct box_layout final : public layout {
  void self_size(area_utils utils) const noexcept override {

    ui_size res{0, 0};

    auto &style = utils.self_style();

    auto requests = utils.get_requests();

    bool end{false};
    for (auto &&rq : requests) {
      if (end) {
        rq.discard();
        continue;
      }

      res.h += rq.style_of().shape.margin.top;
      res.h += rq.value().h;

      if (style.shape.max_size.h != 0 && res.h > style.shape.max_size.h) {
        end = true;
        continue;
      }

      upixel_t w = rq.value().w + rq.style_of().shape.margin.left;
      if (w > res.w) {
        res.w = w;
      }

      rq.apply();
    }

    auto rq = minmax(utils.self_style(), res);

    utils.request_size(rq);

    // мб сделать в стиле std::prindln ?
    utils.log("box - complite");
  };

  void set_childs_position(position_utils utils) const noexcept override {
    auto position = utils.self_position();
    auto def = position;

    auto content = utils.content();

    for (auto &&rq : content) {
      auto &style = rq.style_of();

      position.y += style.shape.margin.top;
      position.x += style.shape.margin.left;
      rq.apply(position);
      auto &size = rq.size_of();
      position.y += size.h;
      position.x = def.x;
    }
  };

  void balancing(balancing_utils utils) const noexcept override {
    if (utils.is_strong_applied()) {
      return;
    }
  };
};
}; // namespace iuic
