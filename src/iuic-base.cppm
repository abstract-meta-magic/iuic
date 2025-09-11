

module;

#include <cstdint>
#include <print>
#include <string_view>
#include <variant>

// Independ User Interface Core

export module iuic.core:base;
export namespace iuic {

struct celement;
struct relement;
struct style;
struct layout;

using uid_t = std::uint64_t;
using pixel_t = int; // swap to int64_t
using upixel_t = unsigned;

struct ui_position {
  pixel_t x, y = 0;
  constexpr auto operator<=>(const ui_position &) const = default;
};

struct ui_size {
  upixel_t h, w = 0;
  constexpr auto operator<=>(const ui_size &) const = default;
};

struct ui_rect {
  ui_position position;
  ui_size size;
  constexpr auto operator<=>(const ui_rect &) const = default;
};

struct indent {
  upixel_t top, bottom, left, right;
  // конструкторы и т.д.
};

struct border_radius {
  // TODO : body
};

// top | bottom = horisontal center
// right | left = vertical center
// TODO : нормальные доки

/*
  Используеться для определения типа выравнивания.
  Может быть спокойно проигнорирован layout.
*/
struct align {
  enum class vertical : std::uint8_t {
    top = 1,
    middle = 2,
    bottom = 3
  } vertical_v{vertical::top};

  enum class horisontal : std::uint8_t {
    left = 10,
    middle = 20,
    right = 30
  } horisontal_v{horisontal::left};

  enum class target : std::uint8_t {
    childs = 100,
    self = 200
  } target{target::childs};

  align &operator=(const align::vertical &v) noexcept {
    vertical_v = v;
    return *this;
  }

  align &operator=(align::vertical &&v) noexcept {
    vertical_v = v;
    return *this;
  }

  align &operator=(align::horisontal &&h) noexcept {
    horisontal_v = h;
    return *this;
  }

  align &operator=(const align::horisontal &h) noexcept {
    horisontal_v = h;
    return *this;
  }

  // Align type to number.
  constexpr std::uint8_t to_num() const noexcept {
    return static_cast<std::uint8_t>(vertical_v) +
           static_cast<std::uint8_t>(horisontal_v);
  };

  // Aling type + target to number.
  constexpr std::uint8_t to_num_full() const noexcept {
    return static_cast<std::uint8_t>(vertical_v) +
           static_cast<std::uint8_t>(horisontal_v) +
           static_cast<std::uint8_t>(target);
  };

  // Align type to number.
  constexpr operator std::uint8_t() const noexcept { return to_num(); };
};

// top-middle
constexpr align align_tm{align::vertical::top, align::horisontal::middle};
// middle-middle
constexpr align align_mm{align::vertical::middle, align::horisontal::middle};
// middle-left
constexpr align align_ml{align::vertical::middle, align::horisontal::left};
// middle-right
constexpr align align_mr{align::vertical::middle, align::horisontal::right};
// bottom-middle
constexpr align align_bm{align::vertical::bottom, align::horisontal::middle};
// top-left
constexpr align align_tl{align::vertical::bottom, align::horisontal::left};
// top-right
constexpr align align_tr{align::vertical::bottom, align::horisontal::right};
// bottom-left
constexpr align align_bl{align::vertical::bottom, align::horisontal::left};
// bottom-right
constexpr align align_br{align::vertical::bottom, align::horisontal::right};

// вынести
struct style_font {
  // ...
};

struct color_t {
  static constexpr color_t get_white() noexcept {
    return {255, 255, 255, 255};
  };

public:
  uint8_t r{0}, g{0}, b{0}, a = {255};
};

// base color for style
// TODO : replace in other fine and module partition
namespace color::css {

struct white : color_t {
  constexpr white() : color_t{255, 255, 255, 255} {}
  explicit constexpr white(std::uint8_t alpha)
      : color_t{255, 255, 255, alpha} {}
};

struct black : color_t {
  constexpr black() : color_t{0, 0, 0, 255} {}
  explicit constexpr black(std::uint8_t alpha) : color_t{0, 0, 0, alpha} {}
};

struct red : color_t {
  constexpr red() : color_t{255, 0, 0, 255} {}
  explicit constexpr red(std::uint8_t alpha) : color_t{255, 0, 0, alpha} {}
};

struct green : color_t {
  constexpr green() : color_t{0, 128, 0, 255} {}
  explicit constexpr green(std::uint8_t alpha) : color_t{0, 128, 0, alpha} {}
};

struct blue : color_t {
  constexpr blue() : color_t{0, 0, 255, 255} {}
  explicit constexpr blue(std::uint8_t alpha) : color_t{0, 0, 255, alpha} {}
};

struct gray : color_t {
  constexpr gray() : color_t{128, 128, 128, 255} {}
  explicit constexpr gray(std::uint8_t alpha) : color_t{128, 128, 128, alpha} {}
};

struct lightgray : color_t {
  constexpr lightgray() : color_t{211, 211, 211, 255} {}
  explicit constexpr lightgray(std::uint8_t alpha)
      : color_t{211, 211, 211, alpha} {}
};

struct darkgray : color_t {
  constexpr darkgray() : color_t{169, 169, 169, 255} {}
  explicit constexpr darkgray(std::uint8_t alpha)
      : color_t{169, 169, 169, alpha} {}
};

struct yellow : color_t {
  constexpr yellow() : color_t{255, 255, 0, 255} {}
  explicit constexpr yellow(std::uint8_t alpha) : color_t{255, 255, 0, alpha} {}
};

struct cyan : color_t {
  constexpr cyan() : color_t{0, 255, 255, 255} {}
  explicit constexpr cyan(std::uint8_t alpha) : color_t{0, 255, 255, alpha} {}
};

struct magenta : color_t {
  constexpr magenta() : color_t{255, 0, 255, 255} {}
  explicit constexpr magenta(std::uint8_t alpha)
      : color_t{255, 0, 255, alpha} {}
};

struct orange : color_t {
  constexpr orange() : color_t{255, 165, 0, 255} {}
  explicit constexpr orange(std::uint8_t alpha) : color_t{255, 165, 0, alpha} {}
};

struct purple : color_t {
  constexpr purple() : color_t{128, 0, 128, 255} {}
  explicit constexpr purple(std::uint8_t alpha) : color_t{128, 0, 128, alpha} {}
};

struct brown : color_t {
  constexpr brown() : color_t{165, 42, 42, 255} {}
  explicit constexpr brown(std::uint8_t alpha) : color_t{165, 42, 42, alpha} {}
};

struct pink : color_t {
  constexpr pink() : color_t{255, 192, 203, 255} {}
  explicit constexpr pink(std::uint8_t alpha) : color_t{255, 192, 203, alpha} {}
};

struct lime : color_t {
  constexpr lime() : color_t{0, 255, 0, 255} {}
  explicit constexpr lime(std::uint8_t alpha) : color_t{0, 255, 0, alpha} {}
};

struct teal : color_t {
  constexpr teal() : color_t{0, 128, 128, 255} {}
  explicit constexpr teal(std::uint8_t alpha) : color_t{0, 128, 128, alpha} {}
};

// use https://www.w3schools.com/cssref/css_colors.php
}; // namespace color::css
namespace color {
struct hex : color_t {
  explicit constexpr hex(std::string_view hex_code)
      : color_t{0, 0, 0, 255} {
          // TODO : IMPL ME
        };
};

}; // namespace color
// вынести

struct ui_none {};
template <typename T> struct ui_initial;

struct ui_inherit {};

struct ui_auto {};

struct ui_background_image {};

using style_background = std::variant<ui_none, color_t, ui_background_image>;

struct style_effects {};

struct style_animations {};
// вынести
struct style_shape {
  ui_size min_size, max_size;

  indent border;

  struct {
    struct {
      border_radius left;
      border_radius right;
    } top;
    struct {
      border_radius left;
      border_radius right;
    } bottom;
  } border_radius;
};

struct style_positioning {

  indent margin;

  indent padding;

  align align;
};

// https://html5book.ru/css-spravochnik.html#part1
struct style {
  // base
  style_shape shape;

  style_positioning positioning;

  style_font font;

  style_background background{ui_none{}};

  style_effects effects;

  style_animations animations;
};

struct text_render_data {
  const style_font &font;
  std::size_t size;
  // ... etc
};

enum class SurfaceDataType { STR, JSON, XML, YAML, TOML, CSV };

/*
  Информация для поиска пользовательской
  поверхности для рисования.Эта информация
  может быть проигнорированна, и если информация
  проигнорированна, то должен быть отрисован
  приметив [frame].
*/
struct surface_static_info {
  std::string type;
  std::string version;
  SurfaceDataType data_type;
};

struct surface_create_info {};

/*
  Простой набор из информации о поверхности и
  прекрипленных к ней данных.
*/
struct surface_render_data {
  const surface_static_info &info;
  // тут должен быть референс
  // так как эта структура пересоздается
  // каждый кадр
  std::string data;
};

struct frame_render_data {
  const style_background &background;
  // ... etc
};

// изображение которые сложнее простого квадрата(background)
struct image_render_data {};

using render_data = std::variant<frame_render_data, surface_render_data,
                                 text_render_data, image_render_data>;

// рисуемый элемент
// relement и некоторые его зависимости
// нужно будет вынисти в отдельную часть
// iuic.core:relement или схожее название
struct relement {
  ui_rect area;      // x,y w,h
  ui_rect clip_area; // простая обрезка
  int z_index;       // слой
  float opacity;     // прозрачность
  // WARNING : большой размер
  render_data data; // метаданные для отрисовки
};

ui_size minmax(const style &st, ui_size val) {

  if (st.shape.max_size.w == 0) {
  } else if (val.w > st.shape.max_size.w) {
    val.w = st.shape.max_size.w;
  }

  if (st.shape.min_size.w == 0) {
  } else if (val.w < st.shape.min_size.w) {
    val.w = st.shape.min_size.w;
  }

  // max\min set
  if (st.shape.max_size.h == 0) {
  } else if (val.h > st.shape.max_size.h) {
    val.h = st.shape.max_size.h;
  }
  if (st.shape.min_size.h == 0) {
  } else if (val.h < st.shape.min_size.h) {
    val.h = st.shape.min_size.h;
  }
  return val;
};

} // namespace iuic
