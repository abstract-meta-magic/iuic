// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.units:ui;
import std;
import iuic.underlying.utils;
import :decl;
import :op;

export namespace iuic::units {

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
  pixel x, y = pixel{0};
  constexpr auto operator<=>(const position &) const = default;
};

struct size {
  upixel w, h = upixel{0};
  constexpr auto operator<=>(const ui::size &) const = default;
};

struct rect {
  union {
    struct {
      pixel x, y;
      upixel w, h;
    };
    struct {
      position position;
      size size;
    };
  };

  std::tuple<pixel, pixel, upixel, upixel> xywh() const {
    return {x, y, w, h};
  };

  struct position center() const {
    return {
        .x = x + (w / 2),
        .y = y + (h / 2),
    };
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

using fill = std::variant<none, color, external::binding>;

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
