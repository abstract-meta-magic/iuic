// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.style;
import std;
import iuic.underlying;

export namespace iuic::policy {
enum class usage {
  unused = 0,
  unspecified,
  optional,
  required,
};
};

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

// limit 128
struct shape {
  units::ui::adaptive::size size;

  units::ui::indent border;

  // 16 bytes free
};

// запрес на динамическое изменение

// limit 64
struct decoration {
  struct {
    struct {
      units::ui::adaptive::unit left{units::ui::none{}};
      units::ui::adaptive::unit right{units::ui::none{}};
    } top;
    struct {
      units::ui::adaptive::unit left{units::ui::none{}};
      units::ui::adaptive::unit right{units::ui::none{}};
    } bottom;
    units::color color;
  } border_radius; // decorations

  units::ui::fill background{units::ui::none{}};
  units::ui::fill foreground{units::ui::none{}};
  units::ui::fill border{units::ui::none{}};
};

enum class position { STATIC, RELATIVE, FIXED, ABSOLUTE, STICKY };

// limit 64
struct transform {

  position position{position::STATIC};

  units::ui::position offset{units::pixel{0}, units::pixel{0}};

  float scale{1.0f};

  // TODO : rotation ?
};

// static - unlimited
struct advanced {
  struct {
    policy::usage usage{policy::usage::unspecified};

    units::ui::adaptive::size min_size{units::ui::none{}, units::ui::none{}},
        max_size{units::ui::none{}, units::ui::none{}};

    units::ui::indent padding{units::ui::none{}, units::ui::none{},
                              units::ui::none{}, units::ui::none{}};

    units::ui::indent margin{units::ui::none{}, units::ui::none{},
                             units::ui::none{}, units::ui::none{}};
  } constraint;

  struct {
    policy::usage usage{policy::usage::unspecified};

    units::weight grow{1000};
    units::weight shrink{1000};
  } adaptive;

  // TODO : in advance shaping v > 2.0
  // CSG is shape in normalized view [1.0f,-1.0f](like Vulkan)
  // in box-model\bounded-box
  // has ratio 1:1,16:9,1:2, etc
  struct {
    policy::usage usage{policy::usage::unspecified};
    // iuic::shape::model model; // CSGM
    // meta ?
  } shape;

  struct {
    units::pixel height;

    // name\famaly ?
  } text;
};

struct decl {
  shape shape;

  decoration decoration;

  transform transform;

  advanced advanced;
};

} // namespace iuic::style
export namespace iuic::style {
struct sheet {
  std::vector<shape> shape;
  std::vector<decoration> decoration;
  std::vector<transform> transform;
  std::vector<advanced> advence;
};

struct index {
  using index_t = std::int16_t;
  index_t shape;
  index_t decoration;
  index_t transform;
  index_t advance;
};

struct value {
  const shape &get_shape() const { return table->shape[index.shape]; };

  const decoration &get_decoration() const {
    return table->decoration[index.decoration];
  };

  const transform &get_transform() const {
    return table->transform[index.transform];
  };

  const advanced &get_advence() const { return table->advence[index.advance]; };

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
