

#include <expected>
module;

#include <cstdint>
#include <iostream>
#include <ranges>
#include <span>
#include <string>
#include <variant>
#include <vector>

// Independ User Interface Core
export module iuic.core:base;

export namespace iuic {

// сделать отдельной частью модуля iuic.core:render
class render_list {};

struct celement;
struct relement;
struct style;
struct layout;

enum class KeyMod {
  None,
  Alt,
  Shift,
  // ...
};

enum class KeyAction { Down, Up, Hold };

using pixel_t = int;
using upixel_t = unsigned;
struct adaptive_t {
  enum type {
    PIXEL,
    // ...etc
  };
  float value;
};

struct ui_position {
  pixel_t x, y = 0;
};

struct ui_size {
  upixel_t w, h = 0;
};

struct ui_rect {
  ui_position position;
  ui_size size;
};

struct indent {
  upixel_t top, bottom, left, right;
  // конструкторы и т.д.
};

// top | bottom = horisontal center
// right | left = vertical center
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

// вынести
struct style_background {
  // ...
};

// вынести
struct style_shape {
  ui_size min_size, max_size;

  indent margin;

  indent padding;

  indent border;
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

struct layout {
  // может позже переписать под ошибки
  using for_err = std::expected<int, int>;
  virtual ~layout() = default;

  // примитивная оценка собственного размера
  virtual ui_size self_size(const style &,
                            std::vector<ui_size> &) const noexcept = 0;
  // приблезительное расположение элементов
  virtual void set_childs_position(ui_rect, align,
                                   std::vector<ui_rect> &) const noexcept = 0;
  // Балансировка очень сложна
  // тут сложно и нужно подумать
  // Сверху приходит ваш ui_rect, а вы должны
  // максимально точно вычислить ui_rect своих дитей
  // может быть вызван более одного раза
  virtual void balancing(ui_rect, std::span<celement> &) const noexcept = 0;
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

enum class calc_statment {
  managed,
  discarted,
};
// вычисляемый элемент сделать полуприватным ?
struct celement {
  // celement(const style &style_) : style{style_} {}
  const style *style{nullptr};
  const layout *layout{nullptr};
  ui_rect calculated_area;
  // layout-proccess

  // ее можно привязать по id элемента не забивая celement мусором
  // render_data linked_data{image_render_data{}}; // метаданные для отрисовки
};

// рисуемый элемент
struct relement {
  ui_rect area;      // x,y w,h
  ui_rect clip_area; // простая обрезка
  int z_index;       // слой
  float opacity;     // прозрачность
  // WARNING : большой размер
  render_data data; // метаданные для отрисовки
};

// базовый приватный layout для всех
// если в стиле отсутствует layout
// для вычислений используется этот
struct box_layout final : layout {
  ui_size self_size(const style &st,
                    std::vector<ui_size> &szs) const noexcept override {
    ui_size res{st.shape.min_size.h, 0};

    if (szs.empty()) {
      return st.shape.min_size;
    }

    for (auto &&sz : szs) {

      if (res.w < sz.w) {
        res.w = sz.w;
      }

      res.h += sz.h;
      std::cout << "[sz]" << sz.h << ":" << sz.w << std::endl;
    }

    if (st.shape.max_size.w == 0) {
    } else if (res.w > st.shape.max_size.w) {
      res.w = st.shape.max_size.w;
    } else if (st.shape.min_size.w == 0) {
    } else if (res.w < st.shape.min_size.w) {
      res.w = st.shape.min_size.w;
    }

    // max\min set
    if (st.shape.max_size.h == 0) {
    } else if (res.h > st.shape.max_size.h) {
      res.h = st.shape.max_size.h;
    } else if (st.shape.min_size.h == 0) {
    } else if (res.h < st.shape.min_size.h) {
      res.h = st.shape.min_size.h;
    }

    std::cout << res.h << ":" << res.w << std::endl;

    return res;
  };

  void set_childs_position(ui_rect r, align a,
                           std::vector<ui_rect> &chs) const noexcept override {

  };

  void balancing(ui_rect self,
                 std::span<celement> &els) const noexcept override {

  };
};
} // namespace iuic
