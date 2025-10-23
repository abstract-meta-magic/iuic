
module;

#include <atomic>
#include <concepts>
#include <cstdint>

export module iuic.kitty_kit;
export import iuic.core;
export import iuic.key_map.base;

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
namespace frappe {};
namespace macchiato {};
namespace mocha {};
}; // namespace kitty_kit::color::catppuccin

export namespace kitty_kit::layout {
using namespace iuic;
struct button : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override;
  bool arrange(frame_arrange_utils utils) const noexcept override;
  void position(frame_position_utils utils) const noexcept override;
};

struct text_button : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override;
  bool arrange(frame_arrange_utils utils) const noexcept override;
  void position(frame_position_utils utils) const noexcept override;
};

struct checkbox : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override;
  bool arrange(frame_arrange_utils utils) const noexcept override;
  void position(frame_position_utils utils) const noexcept override;
};

struct simple_box : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override;
  bool arrange(frame_arrange_utils utils) const noexcept override;
  void position(frame_position_utils utils) const noexcept override;
};

struct short_text : public text_layout {
  measure_result measure(text_measure_utils utils) const noexcept override;
  bool arrange(text_arrange_utils utils) const noexcept override;
};
}; // namespace kitty_kit::layout

export namespace kitty_kit {
enum class theme_e {
  latte,
  frappe,
  macchiato,
  mocha,
};

inline std::atomic<theme_e> theme{theme_e::latte};
using namespace iuic;
using builder = iuic::context::builder;

void button(builder &b, std::invocable<> auto &&callback) {
  static style::decl style_{};
  static layout::button layout_;

  style::ref ref{style_};

  b.frame(
      [&](builder &b) {
        auto uid = b.uid.make(policy::unique{}, "kitty-kit-button");
        b.uid.branch(uid);

        auto ork = b.storage.object.persist(uid, "button-callback");

        b.storage.object.init_if_not(
            ork, [&]() { return std::forward<decltype(callback)>(callback); });

        b.policy.hovered(policy::hovered::propagate);

        using callback_type = std::remove_cvref_t<decltype(callback)>;
        b.event(
            [](event::local::key e) {
              if (e.code == key_map::mouse("left")) {
                e.utils.object.try_visit(
                    e.ork, [](callback_type &callback) { callback(); });
              }
            },
            ork);
      },

      style_, layout_);
};

void text_button(builder &b, trk_t trk, std::invocable<> auto &&callback) {
  static style::decl button_style{};
  static style::decl text_style{};
  static layout::text_button layout_;
  static layout::short_text text_layout;

  b.frame(
      [&](builder &b) {
        auto uid = b.uid.make("kitty-kit-button");
        b.uid.branch(uid);

        auto ork = b.storage.object.persist(uid, "button-callback");

        b.storage.object.init_if_not(
            ork, [&]() { return std::forward<decltype(callback)>(callback); });

        b.policy.hovered(policy::hovered::propagate);

        using callback_type = std::remove_cvref_t<decltype(callback)>;
        b.event(
            [](event::local::key e) {
              if (e.code == key_map::mouse("left")) {
                e.utils.object.try_visit(
                    e.ork, [](callback_type &callback) { callback(); });
              }
            },
            ork);

        b.unit.text(trk, text_style, text_layout);
      },

      button_style, layout_);
};

void text_button(builder &, const text::token &,
                 std::invocable<> auto &&callback);

void lable(builder &b, trk_t trk) {
  static style::decl wrapper_style;
  static style::decl text_box_style;
  static layout::simple_box wrapper_layout;
  static layout::short_text text_box_layout;

  b.unit.frame(
      [&](builder &b) { b.unit.text(trk, text_box_style, text_box_layout); },
      wrapper_style, wrapper_layout);
};

void lable(builder &, const text::token &);

trk_t input(builder &b) {
  struct idle;
  struct focused;
  static style::decl wrapper_style;
  static style::decl text_box_style;
  static layout::simple_box wrapper_layout;
  static layout::short_text text_box_layout;

  static auto idle = pseudo_state::make<struct idle>();
  static auto focused = pseudo_state::make<struct focused>();

  trk_t out;

  b.unit.frame([&](builder &b) {
    auto uid = b.uid.make(policy::unique{}, "kitty-kit-text-input");

    b.uid.branch(uid);
    out = b.storage.text.persist(uid, "text-buffer");

    b.policy.hovered(policy::hovered::propagate);

    // -- state --
    b.state.pseudo_init_value(uid, idle);

    b.event([](event::local::key e) {
      // TODO : key_map
      if (e.code == key_map::mouse("left")) {
        if (auto state = e.utils.state.pseudo(e.uid); state == idle) {
          state = focused;
        }
      }
    });

    b.event([](event::global::key e) {
      if (e.code == key_map::mouse("left")) {
        if (auto state = e.utils.state.pseudo(e.uid);
            state == focused && not e.utils.state.hovered(e.uid)) {
          state = idle;
        }
      }
    });

    b.unit.text(out, text_box_style, text_box_layout);
  });

  return out;
};

bool checkbox(builder &b) {
  struct idle;
  struct active;
  static layout::simple_box layout;
  static style::decl style;

  static auto idle = pseudo_state::make<struct idle>();
  static auto active = pseudo_state::make<struct active>();

  bool out{false};

  b.unit.frame(
      [&](builder &b) {
        auto uid = b.uid.make(policy::unique{}, "kitty-checkbox");

        b.uid.branch(uid);

        if (b.state.pseudo(uid) == active) {
          out = true;
        }

        b.event([](event::local::key e) {
          if (e.code == key_map::mouse("left")) {
            auto state = e.utils.state.pseudo(e.uid);
            state == active ? state = idle : state = active;
          }
        });
      },
      style, layout);

  return out;
};

// list -> item_wrapper -> item
void item_list(builder &, auto begin, auto end, auto call);
}; // namespace kitty_kit
