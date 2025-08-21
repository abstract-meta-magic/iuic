

module;

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <expected>
#include <iostream>
#include <list>
#include <print>
#include <queue>
#include <span>
#include <stack>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

// Independ User Interface Core
export module iuic.core:base;

export namespace iuic {

struct celement;
struct relement;
struct style;
struct layout;

using pixel_t = int; // swap to int64_t
using upixel_t = unsigned;

// in version 0.2
struct adaptive_t {
  enum type {
    PIXEL,
    // ...etc
  };
  float value;
};

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
enum class align : std::uint8_t {
  none, // 0
  top,
  bottom,
  left,
  right,
  hcenter = top | bottom,
  vcenter = left | right,
  center = hcenter | vcenter,
  // ... etc
};

// вынести
struct style_font {
  // ...
};

struct color {
  uint8_t r, g, b, a;
};

// вынести
struct style_background {
  color color;
  // ...
};

// вынести
struct style_shape {
  ui_size min_size, max_size;

  indent margin;

  indent padding;

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

// base color is RGBA\16
// также нужно будет написать API для
// работы со стилями наследование,миксины,etc...
struct style {
  // base
  style_shape shape;

  style_font font;

  style_background background;

  align align;
};

// TOTO : in version 0.2
struct styleset {
  const style &base;
  const style &hover{base};
  const style &active{base};
  const style &focus{base};
  const style &disable{base};
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
