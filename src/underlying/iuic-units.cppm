// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying:units;
import std;

export namespace iuic::units {

// TOTO : use -> enum uid : std::uint64_t;
using uid = std::uint64_t;
// TOTO : use -> enum hash : std::uint64_t;
using hash = std::uint64_t;
using pixel = std::int32_t;
using upixel = std::uint32_t;
using time = std::chrono::time_point<std::chrono::steady_clock>;
using time_duration = std::chrono::duration<double>;

struct percent {
  constexpr auto operator<=>(const percent &) const = default;

  constexpr percent(float value_) noexcept
      : value{value_ >= 0 ? (value_ < 300 ? value_ : 300) : 0} {}

  constexpr operator float() const noexcept { return value * 0.01; }

private:
  float value;
};

struct vh {
  percent value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vh &) const = default;

  constexpr vh(const percent &p) noexcept : value{p} {}
  constexpr vh(percent &&p) noexcept : value{p} {}
  constexpr vh &operator=(const percent &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vh &operator=(percent &&p) noexcept {
    value = p;
    return *this;
  }
};

struct vw {
  percent value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vw &) const = default;

  constexpr vw(const percent &p) noexcept : value{p} {}
  constexpr vw(percent &&p) noexcept : value{p} {}
  constexpr vw &operator=(const percent &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vw &operator=(percent &&p) noexcept {
    value = p;
    return *this;
  }
};

struct em {
  percent value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const em &) const = default;

  constexpr em(const percent &p) noexcept : value{p} {}
  constexpr em(percent &&p) noexcept : value{p} {}
  constexpr em &operator=(const percent &p) noexcept {
    value = p;
    return *this;
  }
  constexpr em &operator=(percent &&p) noexcept {
    value = p;
    return *this;
  }
};

struct rem {
  percent value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const rem &) const = default;

  constexpr rem(const percent &p) noexcept : value{p} {}
  constexpr rem(percent &&p) noexcept : value{p} {}
  constexpr rem &operator=(const percent &p) noexcept {
    value = p;
    return *this;
  }
  constexpr rem &operator=(percent &&p) noexcept {
    value = p;
    return *this;
  }
};
// need px,%,rem,vh,vw

struct color_t {
  static constexpr color_t get_white() noexcept {
    return {255, 255, 255, 255};
  };

public:
  std::uint8_t r{0}, g{0}, b{0}, a = {255};
};

// text font
struct upm {
  //...
};

namespace ui {
struct zorder {
  std::uint16_t group;
  std::uint16_t priority;
};

struct position {
  pixel x, y = 0;
  constexpr auto operator<=>(const position &) const = default;
};
struct size {
  upixel w, h = 0;
  constexpr auto operator<=>(const ui::size &) const = default;
};

struct rect {
  union {
    struct {
      upixel x, y;
      pixel w, h;
    };
    struct {
      position position;
      size size;
    };
  };

  std::tuple<pixel, pixel, upixel, upixel> xywh() const {
    return {x, y, w, h};
  };

  constexpr auto operator<=>(const rect &) const = default;
};

struct area {
  rect bordered;
  rect borderless;

  constexpr auto operator<=>(const area &) const = default;
};

struct none {};

struct inherit {};

struct adapt {};

struct [[deprecated("use external::binding")]] image {};

using background = std::variant<none, color_t, image>;

struct border_radius {
  // TODO : body
};

namespace adaptive {
using unit = std::variant<none, adapt, upixel, percent, vw, vh>;

struct size {
  unit width;
  unit height;
};
} // namespace adaptive

struct indent {
  adaptive::unit top{upixel{0}}, bottom{upixel{0}}, left{upixel{0}},
      right{upixel{0}};
};

struct aspect_ratio {
  std::uint8_t width{0};
  std::uint8_t height{0};
  enum dominant_side_e { AUTO, WIDTH, HEIGHT } dominant_side{AUTO};
};

}; // namespace ui
}; // namespace iuic::units
