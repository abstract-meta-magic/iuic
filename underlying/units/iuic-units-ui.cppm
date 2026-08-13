// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.units:ui;
import std;
import iuic.underlying.external;
import iuic.underlying.erasure;
import :decl;
import :op;

export namespace iuic::units {

// text font
struct upm {
  //...
};

namespace ui {

struct copy {};

struct order {
  std::uint32_t index;
  std::uint32_t layer;
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

  constexpr auto operator<=>(const rect &other) const {
    if (auto cmp = position <=> other.position; cmp != 0) {
      return cmp;
    }
    return size <=> other.size;
  };

  constexpr auto operator==(const rect &other) const {
    return (other <=> other) == 0;
  };

  constexpr auto operator!=(const rect &other) const {
    return (*this == other) != 0;
  };
};

struct area {
  rect bordered;
  rect borderless;

  constexpr auto operator<=>(const area &) const = default;
};

struct none {};

struct inherit {};

struct adapt {};

using fill = std::variant<none, color, iuic::external::instance>;

struct border_radius {
  // TODO : body
};

namespace adaptive {
using unit = std::variant<none, copy, adapt, upixel, percent, vw, vh, segment>;

struct size {
  unit width;
  unit height;
};
} // namespace adaptive

// template use for lock cast
template <typename T> struct tblr_adaptr_crtp {};

struct indent {
  adaptive::unit top{ui::none{}}, bottom{ui::none{}}, left{ui::none{}},
      right{ui::none{}};

  constexpr indent &operator=(adaptive::unit unit) {
    top = unit;
    bottom = unit;
    left = unit;
    right = unit;
    return *this;
  };
};

struct rounding {
  adaptive::unit av{ui::none{}}, ah{ui::none{}}, bv{ui::none{}}, bh{ui::none{}},
      cv{ui::none{}}, ch{ui::none{}}, dv{ui::none{}}, dh{ui::none{}};

  constexpr auto ab() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.av = rhs;
        value.ah = rhs;
        value.bv = rhs;
        value.bh = rhs;
        return *this;
      };

    private:
      rounding &value;
    };

    return _{*this};
  };

  constexpr auto cd() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.cv = rhs;
        value.ch = rhs;
        value.dv = rhs;
        value.dh = rhs;
        return *this;
      };

    private:
      rounding &value;
    };

    return _{*this};
  };

  constexpr auto ac() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.av = rhs;
        value.ah = rhs;
        value.cv = rhs;
        value.ch = rhs;
        return *this;
      };

    private:
      rounding &value;
    };
    return _{*this};
  };

  constexpr auto bd() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.bv = rhs;
        value.bh = rhs;
        value.dv = rhs;
        value.dh = rhs;
        return *this;
      };

    private:
      rounding &value;
    };

    return _{*this};
  };

  constexpr auto h() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.ah = rhs;
        value.bh = rhs;
        value.ch = rhs;
        value.dh = rhs;
        return *this;
      };

    private:
      rounding &value;
    };

    return _{*this};
  };

  constexpr auto v() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.av = rhs;
        value.bv = rhs;
        value.cv = rhs;
        value.dv = rhs;
        return *this;
      };

    private:
      rounding &value;
    };

    return _{*this};
  };

  constexpr auto abcd() {
    struct _ {
      constexpr _(rounding &r) : value{r} {};
      constexpr _ &operator=(adaptive::unit rhs) {
        value.av = rhs;
        value.ah = rhs;
        value.bv = rhs;
        value.bh = rhs;
        value.cv = rhs;
        value.ch = rhs;
        value.dv = rhs;
        value.dh = rhs;
        return *this;
      };

    private:
      rounding &value;
    };

    return _{*this};
  };
};

struct aspect_ratio {
  std::uint8_t width{0};
  std::uint8_t height{0};
  enum dominant_side_e { AUTO, WIDTH, HEIGHT } dominant_side{AUTO};
};

}; // namespace ui
}; // namespace iuic::units
