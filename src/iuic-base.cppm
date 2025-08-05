

module;

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <expected>
#include <iostream>
#include <list>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

// Independ User Interface Core
export module iuic.core:base;

namespace iuic {
// Контекст каждого элемента
// используемый для вычисленией.
struct computing_context;
} // namespace iuic

export namespace iuic {

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

template <typename T>
concept layout_cpt = std::is_base_of_v<layout, T>;

struct area_request {
  constexpr area_request(computing_context *of_, computing_context *to_,
                         ui_size rq) noexcept
      : requiest_value{rq}, of{of_}, to{to_} {}

  constexpr area_request(const area_request &) = default;

  // применить текущее решение
  void apply();

  // применить измененное решение
  void apply(ui_size);

  // выкинуть элемент = игнорировать его
  void discard();

  // отложить разрешение до
  // фазы балансировки
  void defer();

  const style &style_of() const;

  const ui_size &value() const noexcept;

private:
  ui_size requiest_value;
  computing_context *of;
  computing_context *to;
};

struct layout_utils_base {
  layout_utils_base(computing_context *self_,
                    computing_context *parent_) noexcept
      : self{self_}, parent{parent_} {};
  // Обычное сообщение для отладки
  void log(std::string_view message) const noexcept;
  // Предупреждение об исключительной ситвации.
  void warning(std::string_view message) const noexcept;
  // Сообщение об ошибке.
  void error(std::string_view message) const noexcept;

  // получение ссылки на собственный стиль
  const style &self_style() const;

  // получение ссылки на родительский стиль
  const style &parent_style() const;

  // получение ссылки на viewport стиль
  const style &viewport_style() const;

  // отложить
  void defer();

protected: // общие нужды
  computing_context *self{nullptr};
  computing_context *parent{nullptr};

private: // реализация базовых концепций логирования
};

// Структура которая помогает
// при вычислении собственной позиции
struct area_utils : layout_utils_base {
  area_utils(computing_context *self, computing_context *parent) noexcept
      : layout_utils_base{self, parent} {};
  // терминальный метод.
  // потребовать позицию.
  // требования могут быть отклонены,
  // а элеимент помечен тегом [discardet]
  void request_size(ui_size);

  // терминальный метод.
  // получение размеров для
  // элиментов с абсалютным позиционированием,
  // но иерархически пренадлижащим своим элементам.
  void viewport_request_size(ui_size);

  // есть ли запросы на выделение
  bool has_request() const noexcept;

  // следующий запрос
  area_request next_request();

  // терминальный метод.
  // помечает элемент и его детей как discarted
  void self_discard();

  // Пометить оставшиеся запросы как
  // discarted.
  void discard_remaining_requiests();
};

struct position_request {
  constexpr position_request(computing_context *owner_) noexcept
      : owner{owner_} {}

  void apply(ui_position);

  void discard();

  const style &style_of() const noexcept;

  const ui_size &size_of() const noexcept;

private:
  computing_context *owner;
};

struct position_utils : layout_utils_base {
  position_utils(computing_context *self, computing_context *parent) noexcept
      : layout_utils_base{self, parent} {};
  ;

  const ui_position &self_position() const noexcept;

  std::vector<position_request> content();
};

// Набор команд и свойс
// для точного определения позиций
// и размеров
struct balancing_utils : layout_utils_base {
  balancing_utils(computing_context *self, computing_context *parent) noexcept
      : layout_utils_base{self, parent} {};
  ;
  // Вернет запрашиваемый текущем элиментом
  // размер
  ui_size dispatched_requiest_size() const;

  // Был ли выделен размер.
  // Если элемент discarded, то
  // метод вернет false.
  bool is_requiest_applied() const noexcept;

  // Был ли одобрен запрашиваемый размер.
  // Если элемен discarded, то
  // метод вернет false.
  bool is_strong_applied() const noexcept;

  // Вернет размер который одобрил родитель.
  // Если элимент discarded, то
  // метод вернет {0,0}
  ui_size applied_requiest_size() const;

  // ... etc

private: // контекст балансировки
};

struct layout {
  // measure && layout

  // может позже переписать под ошибки
  using for_err = std::expected<int, int>;
  virtual ~layout() = default;

  // примитивная оценка собственного размера
  // можно подумать о предоставлении ограничителя на
  // вычисления размеров относительно родителя
  virtual void self_size(area_utils) const noexcept = 0;
  // приблезительное расположение элементов
  virtual void set_childs_position(position_utils) const noexcept = 0;

  // Балансировка очень сложна
  // тут сложно и нужно подумать
  // Сверху приходит ваш ui_rect, а вы должны
  // максимально точно вычислить ui_rect своих дитей
  // может быть вызван более одного раза
  virtual void balancing(balancing_utils) const noexcept = 0;

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
