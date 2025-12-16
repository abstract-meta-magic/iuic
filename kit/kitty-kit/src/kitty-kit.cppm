export module iuic.kitty_kit;
import std;
export import iuic.core;
export import iuic.key_map.base;

// используются цвета - https://catppuccin.com/palette/
export namespace kitty_kit::color::catppuccin {
using namespace iuic;

namespace latte {

struct rosewater : public color_t {
  explicit constexpr rosewater(std::uint8_t alpha = 255)
      : color_t{220, 138, 120, alpha} {}
};

struct flamingo : public color_t {
  explicit constexpr flamingo(std::uint8_t alpha = 255)
      : color_t{221, 120, 120, alpha} {}
};

struct pink : public color_t {
  explicit constexpr pink(std::uint8_t alpha = 255)
      : color_t{234, 118, 203, alpha} {}
};

struct mauve : public color_t {
  explicit constexpr mauve(std::uint8_t alpha = 255)
      : color_t{136, 57, 239, alpha} {}
};

struct red : public color_t {
  explicit constexpr red(std::uint8_t alpha = 255)
      : color_t{210, 15, 57, alpha} {}
};

struct maroon : public color_t {
  explicit constexpr maroon(std::uint8_t alpha = 255)
      : color_t{230, 69, 83, alpha} {}
};

struct peach : public color_t {
  explicit constexpr peach(std::uint8_t alpha = 255)
      : color_t{254, 100, 11, alpha} {}
};

struct yellow : public color_t {
  explicit constexpr yellow(std::uint8_t alpha = 255)
      : color_t{223, 142, 29, alpha} {}
};

struct green : public color_t {
  explicit constexpr green(std::uint8_t alpha = 255)
      : color_t{64, 160, 43, alpha} {}
};

struct teal : public color_t {
  explicit constexpr teal(std::uint8_t alpha = 255)
      : color_t{23, 146, 153, alpha} {}
};

struct sky : public color_t {
  explicit constexpr sky(std::uint8_t alpha = 255)
      : color_t{4, 165, 229, alpha} {}
};

struct sapphire : public color_t {
  explicit constexpr sapphire(std::uint8_t alpha = 255)
      : color_t{32, 159, 181, alpha} {}
};

struct blue : public color_t {
  explicit constexpr blue(std::uint8_t alpha = 255)
      : color_t{30, 102, 245, alpha} {}
};

struct lavender : public color_t {
  explicit constexpr lavender(std::uint8_t alpha = 255)
      : color_t{114, 135, 253, alpha} {}
};

struct text : public color_t {
  explicit constexpr text(std::uint8_t alpha = 255)
      : color_t{76, 79, 105, alpha} {}
};

struct sub_text_0 : public color_t {
  explicit constexpr sub_text_0(std::uint8_t alpha = 255)
      : color_t{108, 111, 133, alpha} {}
};

struct sub_text_1 : public color_t {
  explicit constexpr sub_text_1(std::uint8_t alpha = 255)
      : color_t{92, 95, 119, alpha} {}
};

struct overlay_0 : public color_t {
  explicit constexpr overlay_0(std::uint8_t alpha = 255)
      : color_t{156, 160, 176, alpha} {}
};

struct overlay_1 : public color_t {
  explicit constexpr overlay_1(std::uint8_t alpha = 255)
      : color_t{140, 143, 161, alpha} {}
};

struct overlay_2 : public color_t {
  explicit constexpr overlay_2(std::uint8_t alpha = 255)
      : color_t{124, 127, 147, alpha} {}
};

struct surface_0 : public color_t {
  explicit constexpr surface_0(std::uint8_t alpha = 255)
      : color_t{204, 208, 218, alpha} {}
};

struct surface_1 : public color_t {
  explicit constexpr surface_1(std::uint8_t alpha = 255)
      : color_t{188, 192, 204, alpha} {}
};

struct surface_2 : public color_t {
  explicit constexpr surface_2(std::uint8_t alpha = 255)
      : color_t{172, 176, 190, alpha} {}
};

struct base : public color_t {
  explicit constexpr base(std::uint8_t alpha = 255)
      : color_t{239, 241, 245, alpha} {}
};

struct mantle : public color_t {
  explicit constexpr mantle(std::uint8_t alpha = 255)
      : color_t{230, 233, 239, alpha} {}
};

struct crust : public color_t {
  explicit constexpr crust(std::uint8_t alpha = 255)
      : color_t{220, 224, 232, alpha} {}
};
}; // namespace latte
namespace frappe {

struct rosewater : public color_t {
  explicit constexpr rosewater(std::uint8_t alpha = 255)
      : color_t{242, 213, 207, alpha} {}
};

struct flamingo : public color_t {
  explicit constexpr flamingo(std::uint8_t alpha = 255)
      : color_t{238, 190, 190, alpha} {}
};

struct pink : public color_t {
  explicit constexpr pink(std::uint8_t alpha = 255)
      : color_t{244, 184, 228, alpha} {}
};

struct mauve : public color_t {
  explicit constexpr mauve(std::uint8_t alpha = 255)
      : color_t{202, 158, 230, alpha} {}
};

struct red : public color_t {
  explicit constexpr red(std::uint8_t alpha = 255)
      : color_t{231, 130, 132, alpha} {}
};

struct maroon : public color_t {
  explicit constexpr maroon(std::uint8_t alpha = 255)
      : color_t{234, 153, 156, alpha} {}
};

struct peach : public color_t {
  explicit constexpr peach(std::uint8_t alpha = 255)
      : color_t{239, 159, 118, alpha} {}
};

struct yellow : public color_t {
  explicit constexpr yellow(std::uint8_t alpha = 255)
      : color_t{229, 200, 144, alpha} {}
};

struct green : public color_t {
  explicit constexpr green(std::uint8_t alpha = 255)
      : color_t{166, 209, 137, alpha} {}
};

struct teal : public color_t {
  explicit constexpr teal(std::uint8_t alpha = 255)
      : color_t{129, 200, 190, alpha} {}
};

struct sky : public color_t {
  explicit constexpr sky(std::uint8_t alpha = 255)
      : color_t{153, 209, 219, alpha} {}
};

struct sapphire : public color_t {
  explicit constexpr sapphire(std::uint8_t alpha = 255)
      : color_t{133, 193, 220, alpha} {}
};

struct blue : public color_t {
  explicit constexpr blue(std::uint8_t alpha = 255)
      : color_t{140, 170, 238, alpha} {}
};

struct lavender : public color_t {
  explicit constexpr lavender(std::uint8_t alpha = 255)
      : color_t{186, 187, 241, alpha} {}
};

struct text : public color_t {
  explicit constexpr text(std::uint8_t alpha = 255)
      : color_t{198, 208, 245, alpha} {}
};

struct sub_text_0 : public color_t {
  explicit constexpr sub_text_0(std::uint8_t alpha = 255)
      : color_t{165, 173, 206, alpha} {}
};

struct sub_text_1 : public color_t {
  explicit constexpr sub_text_1(std::uint8_t alpha = 255)
      : color_t{181, 191, 226, alpha} {}
};

struct overlay_0 : public color_t {
  explicit constexpr overlay_0(std::uint8_t alpha = 255)
      : color_t{115, 121, 148, alpha} {}
};

struct overlay_1 : public color_t {
  explicit constexpr overlay_1(std::uint8_t alpha = 255)
      : color_t{131, 139, 167, alpha} {}
};

struct overlay_2 : public color_t {
  explicit constexpr overlay_2(std::uint8_t alpha = 255)
      : color_t{148, 156, 187, alpha} {}
};

struct surface_0 : public color_t {
  explicit constexpr surface_0(std::uint8_t alpha = 255)
      : color_t{65, 69, 89, alpha} {}
};

struct surface_1 : public color_t {
  explicit constexpr surface_1(std::uint8_t alpha = 255)
      : color_t{81, 87, 109, alpha} {}
};

struct surface_2 : public color_t {
  explicit constexpr surface_2(std::uint8_t alpha = 255)
      : color_t{98, 104, 128, alpha} {}
};

struct base : public color_t {
  explicit constexpr base(std::uint8_t alpha = 255)
      : color_t{48, 52, 70, alpha} {}
};

struct mantle : public color_t {
  explicit constexpr mantle(std::uint8_t alpha = 255)
      : color_t{41, 44, 60, alpha} {}
};

struct crust : public color_t {
  explicit constexpr crust(std::uint8_t alpha = 255)
      : color_t{35, 38, 52, alpha} {}
};
}; // namespace frappe
namespace macchiato {

struct rosewater : public color_t {
  explicit constexpr rosewater(std::uint8_t alpha = 255)
      : color_t{244, 219, 214, alpha} {}
};

struct flamingo : public color_t {
  explicit constexpr flamingo(std::uint8_t alpha = 255)
      : color_t{240, 198, 198, alpha} {}
};

struct pink : public color_t {
  explicit constexpr pink(std::uint8_t alpha = 255)
      : color_t{245, 189, 230, alpha} {}
};

struct mauve : public color_t {
  explicit constexpr mauve(std::uint8_t alpha = 255)
      : color_t{198, 160, 246, alpha} {}
};

struct red : public color_t {
  explicit constexpr red(std::uint8_t alpha = 255)
      : color_t{237, 135, 150, alpha} {}
};

struct maroon : public color_t {
  explicit constexpr maroon(std::uint8_t alpha = 255)
      : color_t{238, 153, 160, alpha} {}
};

struct peach : public color_t {
  explicit constexpr peach(std::uint8_t alpha = 255)
      : color_t{245, 169, 127, alpha} {}
};

struct yellow : public color_t {
  explicit constexpr yellow(std::uint8_t alpha = 255)
      : color_t{238, 212, 159, alpha} {}
};

struct green : public color_t {
  explicit constexpr green(std::uint8_t alpha = 255)
      : color_t{166, 218, 149, alpha} {}
};

struct teal : public color_t {
  explicit constexpr teal(std::uint8_t alpha = 255)
      : color_t{139, 213, 202, alpha} {}
};

struct sky : public color_t {
  explicit constexpr sky(std::uint8_t alpha = 255)
      : color_t{145, 215, 227, alpha} {}
};

struct sapphire : public color_t {
  explicit constexpr sapphire(std::uint8_t alpha = 255)
      : color_t{125, 196, 228, alpha} {}
};

struct blue : public color_t {
  explicit constexpr blue(std::uint8_t alpha = 255)
      : color_t{138, 173, 244, alpha} {}
};

struct lavender : public color_t {
  explicit constexpr lavender(std::uint8_t alpha = 255)
      : color_t{183, 189, 248, alpha} {}
};

struct text : public color_t {
  explicit constexpr text(std::uint8_t alpha = 255)
      : color_t{202, 211, 245, alpha} {}
};

struct sub_text_0 : public color_t {
  explicit constexpr sub_text_0(std::uint8_t alpha = 255)
      : color_t{165, 173, 203, alpha} {}
};

struct sub_text_1 : public color_t {
  explicit constexpr sub_text_1(std::uint8_t alpha = 255)
      : color_t{184, 192, 224, alpha} {}
};

struct overlay_0 : public color_t {
  explicit constexpr overlay_0(std::uint8_t alpha = 255)
      : color_t{110, 115, 141, alpha} {}
};

struct overlay_1 : public color_t {
  explicit constexpr overlay_1(std::uint8_t alpha = 255)
      : color_t{128, 135, 162, alpha} {}
};

struct overlay_2 : public color_t {
  explicit constexpr overlay_2(std::uint8_t alpha = 255)
      : color_t{147, 154, 183, alpha} {}
};

struct surface_0 : public color_t {
  explicit constexpr surface_0(std::uint8_t alpha = 255)
      : color_t{54, 58, 79, alpha} {}
};

struct surface_1 : public color_t {
  explicit constexpr surface_1(std::uint8_t alpha = 255)
      : color_t{73, 77, 100, alpha} {}
};

struct surface_2 : public color_t {
  explicit constexpr surface_2(std::uint8_t alpha = 255)
      : color_t{91, 96, 120, alpha} {}
};

struct base : public color_t {
  explicit constexpr base(std::uint8_t alpha = 255)
      : color_t{36, 39, 58, alpha} {}
};

struct mantle : public color_t {
  explicit constexpr mantle(std::uint8_t alpha = 255)
      : color_t{30, 32, 48, alpha} {}
};

struct crust : public color_t {
  explicit constexpr crust(std::uint8_t alpha = 255)
      : color_t{24, 25, 38, alpha} {}
};
}; // namespace macchiato
namespace mocha {

struct rosewater : public color_t {
  explicit constexpr rosewater(std::uint8_t alpha = 255)
      : color_t{245, 224, 220, alpha} {}
};

struct flamingo : public color_t {
  explicit constexpr flamingo(std::uint8_t alpha = 255)
      : color_t{242, 205, 205, alpha} {}
};

struct pink : public color_t {
  explicit constexpr pink(std::uint8_t alpha = 255)
      : color_t{245, 194, 231, alpha} {}
};

struct mauve : public color_t {
  explicit constexpr mauve(std::uint8_t alpha = 255)
      : color_t{203, 166, 247, alpha} {}
};

struct red : public color_t {
  explicit constexpr red(std::uint8_t alpha = 255)
      : color_t{243, 139, 168, alpha} {}
};

struct maroon : public color_t {
  explicit constexpr maroon(std::uint8_t alpha = 255)
      : color_t{235, 160, 172, alpha} {}
};

struct peach : public color_t {
  explicit constexpr peach(std::uint8_t alpha = 255)
      : color_t{250, 179, 135, alpha} {}
};

struct yellow : public color_t {
  explicit constexpr yellow(std::uint8_t alpha = 255)
      : color_t{249, 226, 175, alpha} {}
};

struct green : public color_t {
  explicit constexpr green(std::uint8_t alpha = 255)
      : color_t{166, 227, 175, alpha} {}
};

struct teal : public color_t {
  explicit constexpr teal(std::uint8_t alpha = 255)
      : color_t{116, 226, 213, alpha} {}
};

struct sky : public color_t {
  explicit constexpr sky(std::uint8_t alpha = 255)
      : color_t{137, 220, 235, alpha} {}
};

struct sapphire : public color_t {
  explicit constexpr sapphire(std::uint8_t alpha = 255)
      : color_t{116, 199, 236, alpha} {}
};

struct blue : public color_t {
  explicit constexpr blue(std::uint8_t alpha = 255)
      : color_t{137, 180, 250, alpha} {}
};

struct lavender : public color_t {
  explicit constexpr lavender(std::uint8_t alpha = 255)
      : color_t{180, 190, 254, alpha} {}
};

struct text : public color_t {
  explicit constexpr text(std::uint8_t alpha = 255)
      : color_t{205, 214, 244, alpha} {}
};

struct sub_text_0 : public color_t {
  explicit constexpr sub_text_0(std::uint8_t alpha = 255)
      : color_t{166, 173, 200, alpha} {}
};

struct sub_text_1 : public color_t {
  explicit constexpr sub_text_1(std::uint8_t alpha = 255)
      : color_t{186, 194, 222, alpha} {}
};

struct overlay_0 : public color_t {
  explicit constexpr overlay_0(std::uint8_t alpha = 255)
      : color_t{108, 112, 134, alpha} {}
};

struct overlay_1 : public color_t {
  explicit constexpr overlay_1(std::uint8_t alpha = 255)
      : color_t{127, 132, 156, alpha} {}
};

struct overlay_2 : public color_t {
  explicit constexpr overlay_2(std::uint8_t alpha = 255)
      : color_t{147, 153, 178, alpha} {}
};

struct surface_0 : public color_t {
  explicit constexpr surface_0(std::uint8_t alpha = 255)
      : color_t{49, 50, 68, alpha} {}
};

struct surface_1 : public color_t {
  explicit constexpr surface_1(std::uint8_t alpha = 255)
      : color_t{69, 71, 90, alpha} {}
};

struct surface_2 : public color_t {
  explicit constexpr surface_2(std::uint8_t alpha = 255)
      : color_t{88, 91, 112, alpha} {}
};

struct base : public color_t {
  explicit constexpr base(std::uint8_t alpha = 255)
      : color_t{30, 30, 46, alpha} {}
};

struct mantle : public color_t {
  explicit constexpr mantle(std::uint8_t alpha = 255)
      : color_t{24, 24, 37, alpha} {}
};

struct crust : public color_t {
  explicit constexpr crust(std::uint8_t alpha = 255)
      : color_t{17, 17, 27, alpha} {}
};
} // namespace mocha
}; // namespace kitty_kit::color::catppuccin

export namespace kitty_kit::layout {
using namespace iuic;
struct text_button : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override;
  bool arrange(frame_arrange_utils utils) const noexcept override;
};

struct simple_box : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override;
  bool arrange(frame_arrange_utils utils) const noexcept override;
};

struct short_text : public text_layout {
  measure_result measure(text_measure_utils utils) const noexcept override;
  iuic::text::glyph::sequence
  arrange(text_arrange_utils utils) const noexcept override;
};
}; // namespace kitty_kit::layout

export namespace kitty_kit {

namespace style {

constexpr inline iuic::style::font::decl font_base;

constexpr inline iuic::style::font::decl font_base_hovered{font_base};

constexpr iuic::style::decl base{[]() {
  iuic::style::decl res{};

  res.shape.min_size.h = iuic::percent_t{100};
  res.shape.min_size.w = iuic::percent_t{100};

  res.decoration.background = color::catppuccin::macchiato::surface_2{};

  res.advence.text.font = font_base;

  return res;
}()};

constexpr iuic::style::decl button{[]() {
  iuic::style::decl res{};

  res.shape.min_size.h = iuic::vh_t{3};
  res.shape.min_size.w = iuic::vh_t{18};
  res.shape.margin.top = iuic::upixel_t{20};
  res.shape.margin.bottom = iuic::upixel_t{20};
  res.shape.margin.left = iuic::upixel_t{20};
  res.shape.margin.right = iuic::upixel_t{20};

  res.decoration.background = color::catppuccin::macchiato::surface_2{};

  res.advence.text.font = font_base;

  return res;
}()};
}; // namespace style

enum class theme_e {
  latte,
  frappe,
  macchiato,
  mocha,
};

inline std::atomic<theme_e> theme{theme_e::latte};
using namespace iuic;
using builder = iuic::scheme::builder;

void button(builder &b, std::invocable<> auto &&callback) {
  static layout::simple_box layout_;

  auto uid = b.uid.make(policy::unique{}, "kitty-kit-button");
  b.frame(
      uid,
      [&](auto &b) {
        using callback_type = std::remove_cvref_t<decltype(callback)>;

        b.memory.persist(uid);

        b.memory.init_if_not(
            uid, [&]() { return std::forward<decltype(callback)>(callback); });

        b.policy.hovered(policy::hovered::propagate);

        if (b.state.hovered(uid)) {
          b.style.override(iuic::style::decoration{
              .background{color::catppuccin::macchiato::surface_0{}}});
        }
      },

      style::button, layout_);
};

void text_button(builder &b, std::string_view text,
                 std::invocable<> auto &&callback) {
  static layout::short_text text_layout;
  static layout::text_button button_layout;

  auto uid = b.uid.make(iuic::policy::unique{}, "text-button");

  static constexpr iuic::state::decl touch{};

  b.element.frame(
      uid,
      [&](builder &b) {
        using callback_type = std::remove_cvref_t<decltype(callback)>;

        b.memory.persist<callback_type>(uid);

        b.memory.init_if_not(
            uid, [&]() { return std::forward<decltype(callback)>(callback); });

        b.policy.hovered(policy::hovered::propagate);

        if (b.state.hovered(uid)) {
          b.style.override(iuic::style::decoration{
              .background{color::catppuccin::macchiato::surface_0{}}});
        }

        b.event([](event::local::key e) static {
          e.utils.memory.try_visit(e.object, [&](callback_type &call) {
            if (e.code == iuic::key_map::mouse("left")) {
              call();
            }
          });
        });

        b.element.text(text::token{text}, style::base, text_layout);
      },
      style::button, button_layout);
}
}; // namespace kitty_kit
