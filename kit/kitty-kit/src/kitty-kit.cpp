

module;
#include <type_traits>
#include <variant>
#include <vector>

module iuic.kitty_kit;

namespace kitty_kit::layout {
measure_result text_button::measure(frame_measure_utils utils) const noexcept {

  auto rqs = utils.get_requests();

  if (rqs.empty() || rqs.size() > 1) {
    return {};
  }

  auto rq = rqs[0].value();

  // do ...

  return {{upixel_t{200}, upixel_t{200}}};
}

bool text_button::arrange(frame_arrange_utils utils) const noexcept {
  auto rqs = utils.get_requests();
  for (auto &&rq : rqs) {

    const auto &rq_value = rq.value();
    auto height = utils.height_upixel_of(rq_value.height);
    auto width = utils.width_upixel_of(rq_value.width);

    rq.apply({width, height});
  }
  return true;
}

// simple-button always empty
void text_button::position(frame_position_utils utils) const noexcept {
  // centring ?
}

measure_result simple_box::measure(frame_measure_utils utils) const noexcept {
  auto style = utils.self_style();

  return {measure_request{.rq{
      .width{style.get_shape().min_size.w},
      .height{style.get_shape().min_size.h},
  }}};
};
bool simple_box::arrange(frame_arrange_utils utils) const noexcept {
  return true;
};
void simple_box::position(frame_position_utils utils) const noexcept {};

measure_result short_text::measure(text_measure_utils utils) const noexcept {

  return {};
};
bool short_text::arrange(text_arrange_utils utils) const noexcept {
  return true;
};
}; // namespace kitty_kit::layout
