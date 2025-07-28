

export module iuic.core:transform;
import :base;

namespace iuic::transform {

constexpr inline void shift_left(celement &el, upixel_t val) {
  el.calculated_area.position.x -= val;
};

constexpr inline void shift_right(celement &el, upixel_t val) {
  el.calculated_area.position.x += val;
};

constexpr inline void shift_top(celement &el, upixel_t val) {
  el.calculated_area.position.y += val;
}

constexpr inline void shift_bottom(celement &el, upixel_t val) {
  el.calculated_area.position.y -= val;
}

constexpr inline void set_position(celement &el, ui_position val) {
  el.calculated_area.position = val;
}

constexpr inline void scale(celement &el, float val) {
  // TODO : version 0.2
}
constexpr inline void rotate(celement &el, float val) {
  // TODO : version 0.2
}
}; // namespace iuic::transform
