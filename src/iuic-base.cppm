

module;

#include <concepts>
#include <cstdint>
#include <deque>
#include <expected>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

// Independ User Interface Core
export module iuic.core:base;

export namespace iuic {

// сделать отдельной частью модуля iuic.core:render

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

// Унифицированный код нажатых кнопок
struct key_code {};
namespace keymap::en {
key_code qwerty(std::same_as<std::string_view> auto... args) {
  // TODO : Make key_cade ctor
  return {};
};

key_code qwerty(std::same_as<const char *> auto... args) {
  return qwerty(std::string_view{args}...);
};
} // namespace keymap::en

enum class KeyAction { Down, Up, Hold };

enum class PointerAction { Move, In, Out };

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
};

struct ui_size {
  upixel_t h, w = 0;
};

struct ui_rect {
  ui_position position;
  ui_size size;
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

template <typename T>
concept layout_cpt = std::is_base_of_v<layout, T>;

/*
 Невладеющий объект.
 Можно оставить в chs элементы, и они
 будут помечены как discarted
 */
struct childs_set final {
  constexpr childs_set(std::vector<celement> *c, std::deque<size_t> *ch)
      : container{c}, chs{ch} {};
  constexpr bool has_next() const noexcept { return not chs->empty(); };
  // получить элемент и убрать из стака значение
  celement &get();
  // получить элемент и убрать из стака значение
  const celement &get() const;

private:
  std::vector<celement> *container;
  std::deque<size_t> *chs;
};

struct layout {
  // может позже переписать под ошибки
  using for_err = std::expected<int, int>;
  virtual ~layout() = default;

  // примитивная оценка собственного размера
  virtual ui_size self_size(const style &,
                            const childs_set chs) const noexcept = 0;
  // приблезительное расположение элементов
  virtual void set_childs_position(const celement &,
                                   childs_set chs) const noexcept = 0;
  // Балансировка очень сложна
  // тут сложно и нужно подумать
  // Сверху приходит ваш ui_rect, а вы должны
  // максимально точно вычислить ui_rect своих дитей
  // может быть вызван более одного раза
  virtual void balancing(const celement &, childs_set chs) const noexcept = 0;

  template <layout_cpt T> static constexpr const layout &instance() {
    static constexpr T _{};
    return _;
  };
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

// вычисляемый элемент сделать полуприватным ?
struct celement {
  /*
    Позже можно будет прописать
    индивидуальную обработку
  enum : std::uint8_t {
    invalid,
    discarded,
    absolute,
    parent_of,
    dynamic_childs,
  } properties;
  */
  const size_t id{0};
  const style *style{nullptr};
  ui_rect calculated_area;
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

ui_size minmax(const style &st, ui_size val) {

  if (st.shape.max_size.w == 0) {
  } else if (val.w > st.shape.max_size.w) {
    val.w = st.shape.max_size.w;
  } else if (st.shape.min_size.w == 0) {
  } else if (val.w < st.shape.min_size.w) {
    val.w = st.shape.min_size.w;
  }

  // max\min set
  if (st.shape.max_size.h == 0) {
  } else if (val.h > st.shape.max_size.h) {
    val.h = st.shape.max_size.h;
  } else if (st.shape.min_size.h == 0) {
  } else if (val.h < st.shape.min_size.h) {
    val.h = st.shape.min_size.h;
  }
  return val;
};

} // namespace iuic
