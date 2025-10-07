

module;

#include <cstdint>
#include <string>
#include <variant>

// Independ User Interface Core

export module iuic.core:base;
export namespace iuic {
struct relement;
struct style;

// using hash_t
// using srk
using uid_t = std::uint64_t;
using pixel_t = std::int32_t;
using upixel_t = std::uint32_t;
// type for angl

struct percent_t {
  constexpr auto operator<=>(const percent_t &) const = default;

  constexpr percent_t(float value_) noexcept
      : value{value_ >= 0 ? (value_ < 300 ? value_ : 300) : 0} {}

  constexpr operator float() const noexcept { return value * 0.01; }

private:
  float value;
};

struct vh_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vh_t &) const = default;

  constexpr vh_t(const percent_t &p) noexcept : value{p} {}
  constexpr vh_t(percent_t &&p) noexcept : value{p} {}
  constexpr vh_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vh_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};
struct vw_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const vw_t &) const = default;

  constexpr vw_t(const percent_t &p) noexcept : value{p} {}
  constexpr vw_t(percent_t &&p) noexcept : value{p} {}
  constexpr vw_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr vw_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct em_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const em_t &) const = default;

  constexpr em_t(const percent_t &p) noexcept : value{p} {}
  constexpr em_t(percent_t &&p) noexcept : value{p} {}
  constexpr em_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr em_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};

struct rem_t {
  percent_t value;
  constexpr operator float() const noexcept { return value; }
  constexpr auto operator<=>(const rem_t &) const = default;

  constexpr rem_t(const percent_t &p) noexcept : value{p} {}
  constexpr rem_t(percent_t &&p) noexcept : value{p} {}
  constexpr rem_t &operator=(const percent_t &p) noexcept {
    value = p;
    return *this;
  }
  constexpr rem_t &operator=(percent_t &&p) noexcept {
    value = p;
    return *this;
  }
};
// need px,%,rem,vh,vw

struct ui_position {
  pixel_t x, y = 0;
  constexpr auto operator<=>(const ui_position &) const = default;
};

struct z_order_t {
  std::uint16_t group;
  std::uint16_t priority;
};

// width | hieght
struct ui_size {
  upixel_t w, h = 0;
  constexpr auto operator<=>(const ui_size &) const = default;
};

struct ui_rect {
  ui_position position;
  ui_size size;
  constexpr auto operator<=>(const ui_rect &) const = default;
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

struct ui_none {};
template <typename T> struct ui_initial;

struct ui_inherit {};

struct ui_auto {};

struct ui_background_image {};

using style_background = std::variant<ui_none, color_t, ui_background_image>;

struct style_effects {};

struct style_animations {};

using ui_adaptive_unit =
    std::variant<ui_auto, upixel_t, percent_t, vw_t, vh_t, rem_t>;

struct indent {
  ui_adaptive_unit top{upixel_t{0}}, bottom{upixel_t{0}}, left{upixel_t{0}},
      right{upixel_t{0}};
};

struct border_radius {
  // TODO : body
};

struct ui_adaptive_size {
  ui_adaptive_unit w{upixel_t{0}};
  ui_adaptive_unit h{upixel_t{0}};
};

struct aspect_ratio {
  std::uint8_t width{0};
  std::uint8_t height{0};
  enum dominant_side_e { AUTO, WIDTH, HEIGHT } dominant_side{AUTO};
};

struct style_shape {
  ui_adaptive_size min_size, max_size;

  indent border;

  indent padding;

  aspect_ratio aspect_patio;

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

  float grow{0};

  float shrink{0};
};

struct style_positioning {

  indent margin;

  align align;

  enum position_type_e { STATIC, RELATIVE, ABSOLUTE, FIXED } type;

  // TODO : добавить адаптив
  ui_position offset{0, 0};

  using enum position_type_e;
};

// сделать нормальное наследование свойств ui_inherit<&style_font>...

// https://html5book.ru/css-spravochnik.html#part1
struct style {
  std::string stclass{"none"};

  upixel_t ephemeral_value{14};

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

struct frame_render_data {
  const style_background &background;
  // ... etc
};

// изображение которые сложнее простого квадрата(background)
struct image_render_data {};

using render_data = std::variant<frame_render_data, text_render_data>;

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
namespace policy {
enum class hovered : std::uint8_t {
  none,
  propagate, // true -> go
  block,     // true -> stop
};

enum class event : std::uint8_t { propagate, block };
}; // namespace policy

} // namespace iuic
