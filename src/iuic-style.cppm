// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:style;
import std;
import iuic.underlying;
import :text.font;

export namespace iuic::style {

enum class sid : std::uint64_t {};

constexpr sid &operator++(sid &value) noexcept {
  ++reinterpret_cast<std::uint64_t &>(value);
  return value;
};

constexpr sid operator++(sid &value, int) noexcept {
  auto res = value;
  ++reinterpret_cast<std::uint64_t &>(value);
  return res;
};

constexpr sid &operator--(sid &value) noexcept {
  --reinterpret_cast<std::uint64_t &>(value);
  return value;
};

constexpr sid operator--(sid &value, int) noexcept {
  auto res = value;
  --reinterpret_cast<std::uint64_t &>(value);
  return res;
};

constexpr sid operator+(sid lhs, sid rhs) noexcept {
  return sid{std::to_underlying(lhs) + std::to_underlying(rhs)};
};

constexpr sid operator-(sid lhs, sid rhs) noexcept {
  return sid{std::to_underlying(lhs) - std::to_underlying(rhs)};
};

namespace font {

struct decl {
  consteval decl() = default;
  constexpr ~decl() = default;
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
  units::ui::adaptive::size min_size, max_size;

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
    units::color color;
  } border_radius; // decorations

  units::ui::fill background{units::ui::none{}};
  units::ui::fill foreground{units::ui::none{}};
  units::ui::fill border{units::ui::none{}};
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

  float scale{1.0f};
};

// static - unlimited
struct advence {
  struct {
    bool used{false};

    float grow{1.0f};

    float shrink{1.0f};
  } adaptive;

  struct {
    units::pixel height;

    font::cref font;

  } text;
};

struct decl {
  shape shape;

  decoration decoration;

  transform transform;

  advence advence;
};

} // namespace iuic::style
namespace iuic::style {
struct sheet {
  std::vector<shape> shape;
  std::vector<decoration> decoration;
  std::vector<transform> transform;
  std::vector<advence> advence;
};

struct index {
  using index_t = std::int16_t;
  index_t shape;
  index_t decoration;
  index_t transform;
  index_t advance;
};

export struct value {
  const shape &get_shape() const { return table->shape[index.shape]; };

  const decoration &get_decoration() const {
    return table->decoration[index.decoration];
  };

  const transform &get_transform() const {
    return table->transform[index.transform];
  };

  const advence &get_advence() const { return table->advence[index.advance]; };

  value(index index_, const sheet *sheet_) : index{index_}, table{sheet_} {};

  value() : index{0}, table{nullptr} {}

  value(std::nullptr_t) : index{0}, table{nullptr} {}

  bool valid() const { return table; };

  operator bool() const { return valid(); };

private:
  index index;
  const sheet *table;
};
}; // namespace iuic::style
