module iuic.kitty_kit;
import std;
import iuic.core;

namespace kitty_kit::layout {
iuic::layout::measure::result
simple_box::measure(iuic::layout::measure::frame_utils utils) noexcept {
  auto self_style = utils.style_of(utils.self());
  auto &self_shape = self_style.get_shape();

  std::print("Hi");

  for (auto &&ch : utils.childs_range()) {
    std::visit(
        [](auto &value) {
          using type = std::remove_cvref_t<decltype(value)>;

          if constexpr (std::same_as<type, units::upixel>) {

          } else if constexpr (std::same_as<type, units::vw>) {

          } else if constexpr (std::same_as<type, units::vh>) {
          }
        },
        ch.measure.height);
  }
  return {0, 0};
};

bool simple_box::arrange(iuic::layout::arrange::frame_utils utils) noexcept {

  return false;
};

iuic::layout::measure::result
text_button::measure(iuic::layout::measure::frame_utils utils) noexcept {

  return {0, 0};
};

bool text_button::arrange(iuic::layout::arrange::frame_utils utils) noexcept {

  return false;
};

iuic::layout::measure::result
short_text::measure(iuic::layout::measure::text_utils utils) noexcept {

  return {0, 0};
};

iuic::text::glyph::sequence
short_text::arrange(iuic::layout::arrange::text_utils utils) noexcept {

  return {};
};

} // namespace kitty_kit::layout
