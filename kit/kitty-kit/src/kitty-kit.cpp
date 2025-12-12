
module;
#include <expected>
#include <print>
#include <type_traits>
#include <variant>
#include <vector>

module iuic.kitty_kit;

namespace kitty_kit::layout {
measure_result text_button::measure(frame_measure_utils utils) const noexcept {

  auto rqs = utils.get_requests();
  auto style = utils.self_style();

  if (rqs->valid()) {
    return {};
  }

  auto &rq = *rqs->get();

  // do ...

  return {{style.get_shape().min_size.w, style.get_shape().min_size.h}};
}

bool text_button::arrange(frame_arrange_utils utils) const noexcept {
  auto rqs = utils.get_requests();
  for (auto &&rq : rqs->range()) {

    auto height = utils.height_upixel_of(rq.size.height);
    auto width = utils.width_upixel_of(rq.size.width);

    utils.attach(rq, ui_rect{0, 0, width, height});
  }
  return true;
}

measure_result simple_box::measure(frame_measure_utils utils) const noexcept {
  auto style = utils.self_style();

  return {measure_request{.rq{
      .width{style.get_shape().min_size.w},
      .height{style.get_shape().min_size.h},
  }}};
};
bool simple_box::arrange(frame_arrange_utils utils) const noexcept {
  auto rqs = utils.get_requests();
  for (auto &&rq : rqs->range()) {
    utils.discard(rq);
  }
  return true;
};

measure_result short_text::measure(text_measure_utils utils) const noexcept {
  if (utils.get_tokens().empty()) {
    return {std::unexpected{measure_err{}}};
  }
  upixel_t width = utils.get_tokens()[0].text.size() * 6;

  return {{width, upixel_t{12}}};
};

iuic::text::glyph::sequence
short_text::arrange(text_arrange_utils utils) const noexcept {

  auto size = utils.self_rect();
  auto position = utils.self_position();
  // position ?
  auto tokens = utils.get_tokens();
  auto style = utils.self_style();

  auto &atlas = utils.get_atlas(style.get_advance().text.font);
  auto &decoder = atlas.get_decoder();

  // 20 glyph lock
  std::vector<iuic::text::glyph::placement> pl{};

  for (auto &token : tokens) {
    auto indexes = decoder.decode(token);
    for (auto &index : indexes.value()) {
      pl.push_back({.id = index, .position = position});
      position.x += 6;
    }
  }

  return utils.capture_glyphs(pl);
};
// scroll_box
}; // namespace kitty_kit::layout
