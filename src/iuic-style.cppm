// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:style;
import std;
import iuic.underlying;
import :text.font;

export namespace iuic::style {

namespace font {

struct decl {
  consteval decl() = default;
  consteval decl(const decl &other) : self{this}, underlying{&other} {};
  consteval decl(const decl &underlying_, const decl &adjacent_)
      : self{this}, underlying{&underlying_}, adjacent{&adjacent_} {};

  constexpr std::size_t get_index() const { return std::size_t(self); };

  constexpr const decl *get_underlying() const { return underlying; };

  constexpr const decl *get_adjacent() const { return adjacent; };

  constexpr bool has_underlying() const { return underlying != self; };

  constexpr bool has_adjacent() const { return adjacent != self; };

private:
  const decl *self{this};
  const decl *underlying{this};
  const decl *adjacent{this};
};

constexpr decl base{};

struct cref {
  constexpr cref() : decl{&base} {}
  constexpr cref(const decl *d) : decl{d} {}
  constexpr cref(const decl &d) : decl{&d} {}
  constexpr cref(cref &&) = default;
  constexpr cref(const cref &) = default;
  constexpr cref &operator=(cref &&) = default;
  constexpr cref &operator=(const cref &) = default;

  constexpr cref &operator=(const decl &d) {
    decl = &d;
    return *this;
  };

  constexpr cref &operator=(const decl *d) {
    decl = d;
    return *this;
  };

  constexpr operator const decl &() const {
    if (decl) {
      return *decl;
    } else {
      return base;
    }
  }

  constexpr const decl &value() const { return *this; };

  constexpr const decl &operator*() { return *this; }

  constexpr const decl &operator->() { return *this; }

  constexpr bool operator==(const cref &other) const noexcept {
    return decl == other.decl;
  };

private:
  const decl *decl;
};

}; // namespace font

// limit 128
struct shape {
  units::ui::adaptive_size min_size, max_size;

  units::ui::indent border;

  units::ui::indent padding;

  units::ui::indent margin;
};

// limit 64
struct decoration {
  // в радианах ?
  struct {
    struct {
      units::ui::border_radius left;
      units::ui::border_radius right;
    } top;
    struct {
      units::ui::border_radius left;
      units::ui::border_radius right;
    } bottom;
    units::color_t color;
  } border_radius; // decorations

  // TOTO : replace to external::bind
  units::ui::background background{units::ui::none{}};

  units::color_t foreground{units::color_t{0, 0, 0, 0}};
  units::color_t border{units::color_t{0, 0, 0, 0}};
};

enum class position { STATIC, RELATIVE, FIXED, ABSOLUTE, STICKY };

enum class valign { TOP, MIDDLE, BOTTOM };

enum class halign { LEFT, MIDDLE, RIGHT };

// limit 64
struct transform {
  valign valign{valign::TOP};

  halign halign{halign::LEFT};

  position position{position::STATIC};

  units::ui::position offset{0, 0};

  float scale;
};

struct text {
  units::pixel height;

  font::cref font;

  // other ...
};

// static - unlimited
struct advance {
  float grow{0}; // static

  float shrink{0}; // static

  units::upixel ephemeral_value{14};

  text text;
};

struct decl {
  shape shape;

  decoration decoration;

  transform transform;

  advance advence;
};

// Невладеющий объект.
// Может быть провисшей.
// for relement\calculation
struct cref {
public: // get's
  const shape &get_shape() const {
    if (shape_override_ptr) {
      return *shape_override_ptr;
    } else {
      return base_ptr->shape;
    };
  };

  const decoration &get_decoration() const {
    if (decoration_override_ptr) {
      return *decoration_override_ptr;
    } else {
      return base_ptr->decoration;
    };
  };

  const transform &get_transphorm() const {
    if (transform_override_ptr) {
      return *transform_override_ptr;
    } else {
      return base_ptr->transform;
    };
  };

  const advance &get_advance() const { return base_ptr->advence; };

  cref(const decl *decl) : base_ptr{decl} {};

  cref(const decl &decl) : base_ptr{std::addressof(decl)} {};

protected: //
  const decl *base_ptr;

  const shape *shape_override_ptr{nullptr}; // null

  const decoration *decoration_override_ptr{nullptr}; // null

  const transform *transform_override_ptr{nullptr}; // null
};

struct ref : public cref {
  ref(const decl *decl) : cref{decl} {};

  ref(const decl &decl) : cref{std::addressof(decl)} {};

  void override(const shape *shape) { shape_override_ptr = shape; };

  void override(const decoration *decoration) {
    decoration_override_ptr = decoration;
  };

  void override(const transform *transform) {
    transform_override_ptr = transform;
  };
};
}; // namespace iuic::style
