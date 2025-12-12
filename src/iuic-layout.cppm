

module;

#include <expected>
#include <memory>
#include <memory_resource>
#include <print>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
// TODO : может переименовать в layout.utils ?
export module iuic.core:layout;
import :base;
import :layout.def;
import :text.token;
import :text.buff;
import :text.present;
import :text.fontset;
import :computing.kernel;

namespace iuic {

struct layout_utils_base {
  layout_utils_base(computing::kernel_hardware &kernel_,
                    computing::element e_) noexcept
      : kernel{kernel_}, element{e_} {};
  // Обычное сообщение для отладки
  void log(std::string_view message) const noexcept;
  // Предупреждение об исключительной ситвации.
  void warning(std::string_view message) const noexcept;
  // Сообщение об ошибке.
  void error(std::string_view message) const noexcept;

  // получение ссылки на собственный стиль
  style::cref self_style() const;

  style::cref style_of(computing::element el) const;

  // получение ссылки на родительский стиль
  style::cref parent_style() const;

  // получение ссылки на viewport стиль
  style::cref root_style() const;

  ui_size root_size() const;

  upixel_t rem(rem_t) const noexcept;

  upixel_t vh(vh_t) const noexcept;

  upixel_t vw(vw_t) const noexcept;

  // отложить
  void defer();

  void discard(const computing::request &);

protected: // общие нужды
  computing::kernel_hardware &kernel;
  computing::element element;

private: // реализация базовых концепций логирования
};

// Структура которая помогает
// при вычислении собственной позиции
struct frame_measure_utils : layout_utils_base {

  frame_measure_utils(computing::kernel_hardware &kernel,
                      computing::element e_) noexcept;

  std::unique_ptr<virtual_iterator<const computing::request>> get_requests();

  enum err_r { AUTO, PERCENT, ERR };

  // TODO : Rename
  std::expected<upixel_t, err_r>
  upixel_of(const ui_adaptive_unit &utils) const noexcept {
    return std::visit(
        [this](auto &value) -> std::expected<upixel_t, err_r> {
          using type = std::remove_cvref_t<decltype(value)>;
          if constexpr (std::same_as<type, upixel_t>) {
            return value;
          } else if constexpr (std::same_as<type, percent_t>) {
            return std::unexpected{err_r::PERCENT};
          } else if constexpr (std::same_as<type, vw_t>) {
            return vw(value);
          } else if constexpr (std::same_as<type, vh_t>) {
            return vh(value);
          } else if constexpr (std::same_as<type, rem_t>) {
            return rem(value);
          } else {
            return std::unexpected{err_r::ERR};
          };
        },
        utils);
  };
};

// Набор команд и свойс
// для точного определения позиций
// и размеров
struct frame_arrange_utils : layout_utils_base {
  frame_arrange_utils(computing::kernel_hardware &kernel_hardware_,
                      computing::element e_) noexcept
      : layout_utils_base{kernel_hardware_, e_} {};

  ui_rect self_area() const;

  std::unique_ptr<virtual_iterator<const computing::request>> get_requests();

  void attach(const computing::request &, ui_rect);

  void attach(const computing::request &, ui_rect, ui_rect);

  enum class side_e { WIDTH, HEIGHT };

  upixel_t width_upixel_of(const ui_adaptive_unit &unit) const noexcept {
    return std::visit(
        [this](auto &value) -> upixel_t {
          using type = std::remove_cvref_t<decltype(value)>;

          if constexpr (std::same_as<type, upixel_t>) {
            return value;
          } else if constexpr (std::same_as<type, percent_t>) {
            return self_area().size.w * value;
          } else if constexpr (std::same_as<type, vh_t>) {
            return vh(value);
          } else if constexpr (std::same_as<type, vw_t>) {
            return vw(value);
          } else if constexpr (std::same_as<type, rem_t>) {
            return rem(value);
          } else {
            return 0;
          }
        },
        unit);
  }

  upixel_t height_upixel_of(const ui_adaptive_unit &value_) const noexcept {
    return std::visit(
        [this](auto &val) -> upixel_t {
          using type = std::remove_cvref_t<decltype(val)>;

          if constexpr (std::same_as<type, upixel_t>) {
            return val;
          } else if constexpr (std::same_as<type, percent_t>) {
            return self_area().size.h * val;
          } else if constexpr (std::same_as<type, vh_t>) {
            return vh(val);
          } else if constexpr (std::same_as<type, vw_t>) {
            return vw(val);
          } else if constexpr (std::same_as<type, rem_t>) {
            return rem(val);
          } else {
            return 0;
          }
        },
        value_);
  }
};

struct text_measure_utils : layout_utils_base {
  text_measure_utils(computing::kernel_hardware &kernel_hardware_,
                     computing::element e_, const text::token::sequence &sq_,
                     const text::fontslot &font_)
      : layout_utils_base{kernel_hardware_, e_}, sq{sq_}, font{font_} {};

  text::token::sequence get_tokens() const;

  const text::glyph::atlas &get_atlas(style::font::cref);

private:
  text::token::sequence sq;
  const text::fontslot &font;
};

struct text_arrange_utils : layout_utils_base {
  text_arrange_utils(computing::kernel_hardware &kernel_hardware_,
                     computing::element e_, text::token::sequence sq_,
                     const text::fontslot &font_,
                     std::pmr::memory_resource &tmp_)
      : layout_utils_base{kernel_hardware_, e_}, sq{sq_}, font{font_},
        tmp_resource{tmp_} {}

  ui_rect self_rect() const noexcept;

  ui_position self_position() const noexcept;

  const text::glyph::atlas &get_atlas(style::font::cref);

  text::token::sequence get_tokens() const;

  text::glyph::sequence capture_glyphs(text::glyph::sequence sq) const;
  // make present ...
private:
  text::token::sequence sq;
  const text::fontslot &font;
  std::pmr::memory_resource &tmp_resource;
  static constexpr std::string_view err_token{"err..."};
};
} // namespace iuic
