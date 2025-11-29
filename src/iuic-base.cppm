

module;

#include <chrono>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// Independ User Interface Core

export module iuic.core:base;

namespace iuic {
void advance(auto &);
};

export namespace iuic {

// using hash_t
// using srk
using uid_t = std::uint64_t;
namespace uid {
struct anchor {
  consteval anchor() = default;
  anchor(const anchor &) = delete;
  anchor &operator=(const anchor &) = delete;
  anchor(anchor &&) = delete;
  anchor &operator=(anchor &&) = delete;
};
}; // namespace uid
using pixel_t = std::int32_t;
using upixel_t = std::uint32_t;
using time_t = std::chrono::time_point<std::chrono::steady_clock>;
using time_duration_t = std::chrono::duration<double>;

// type for angl

struct percent_t {
  constexpr auto operator<=>(const percent_t &) const = default;

  constexpr percent_t(float value_) noexcept
      : value{value_ >= 0 ? (value_ < 300 ? value_ : 300) : 0} {}

  constexpr operator float() const noexcept { return value * 0.01; }

private:
  float value;
};

struct vh_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vh_t &) const = default;

  constexpr vh_t(const percent_t &p) noexcept : value{p} {}
  constexpr vh_t(percent_t &&p) noexcept : value{p} {}
  constexpr vh_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vh_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct vw_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vw_t &) const = default;

  constexpr vw_t(const percent_t &p) noexcept : value{p} {}
  constexpr vw_t(percent_t &&p) noexcept : value{p} {}
  constexpr vw_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vw_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct em_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const em_t &) const = default;

  constexpr em_t(const percent_t &p) noexcept : value{p} {}
  constexpr em_t(percent_t &&p) noexcept : value{p} {}
  constexpr em_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr em_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct rem_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const rem_t &) const = default;

  constexpr rem_t(const percent_t &p) noexcept : value{p} {}
  constexpr rem_t(percent_t &&p) noexcept : value{p} {}
  constexpr rem_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr rem_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};
// need px,%,rem,vh,vw

struct ui_position {
  pixel_t x, y = 0;
  constexpr auto operator<=>(const ui_position &) const = default;
};

struct z_order_t {
  std::uint16_t group;
  std::uint16_t priority;
};

// width | hieght
struct ui_size {
  upixel_t w, h = 0;
  constexpr auto operator<=>(const ui_size &) const = default;
};

struct ui_rect {
  ui_position position;
  ui_size size;
  std::tuple<pixel_t, pixel_t, upixel_t, upixel_t> xywh() const {
    return {position.x, position.y, size.w, size.h};
  };
  constexpr auto operator<=>(const ui_rect &) const = default;
};

struct color_t {
  static constexpr color_t get_white() noexcept {
    return {255, 255, 255, 255};
  };

public:
  uint8_t r{0}, g{0}, b{0}, a = {255};
};

struct ui_none {};

struct ui_inherit {};

struct ui_auto {};

struct ui_background_image {};

using style_background = std::variant<ui_none, color_t, ui_background_image>;

using ui_adaptive_unit =
    std::variant<ui_auto, upixel_t, percent_t, vw_t, vh_t, rem_t>;

struct indent {
  ui_adaptive_unit top{upixel_t{0}}, bottom{upixel_t{0}}, left{upixel_t{0}},
      right{upixel_t{0}};
};

struct border_radius {
  // TODO : body
};

struct ui_adaptive_size {
  ui_adaptive_unit w{upixel_t{0}};
  ui_adaptive_unit h{upixel_t{0}};
};

struct aspect_ratio {
  std::uint8_t width{0};
  std::uint8_t height{0};
  enum dominant_side_e { AUTO, WIDTH, HEIGHT } dominant_side{AUTO};
};

// text font
struct upm_t {
  //...
};

} // namespace iuic
