
module;
#include <iostream>
#include <print>

export module iuic.core:layout.box;
import :base;

namespace iuic {
// базовый приватный layout для всех
// если в стиле отсутствует layout
// для вычислений используется этот
struct box_layout final : public layout {
  ui_size self_size(const style &st,
                    const childs_set chs) const noexcept override {

    ui_size res{0, 0};

    if (not chs.has_next()) {
      return st.shape.min_size;
    }

    for (; chs.has_next();) {
      const auto &ch = chs.get();

      const auto &size = ch.calculated_area.size;

      if (res.w < size.w) {
        res.w = size.w + ch.style->shape.margin.left;
      }

      res.h += size.h + ch.style->shape.margin.top;

      std::println("size -> h : {} | w : {}", size.h, size.w);
    }

    return minmax(st, res);
  };

  void set_childs_position(const celement &self,
                           childs_set chs) const noexcept override {
    auto r = self.calculated_area.position;
    for (; chs.has_next();) {
      auto &ch = chs.get();

      auto &position = ch.calculated_area.position;
      r.y += ch.style->shape.margin.top;
      position = r;

      position.x += ch.style->shape.margin.left;
      r.y += ch.calculated_area.size.h;

      std::println("position : x={} , y={}", position.x, position.y);
    }
  };

  void balancing(const celement &self, childs_set chs) const noexcept override {

  };
};
}; // namespace iuic
