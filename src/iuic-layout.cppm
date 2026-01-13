// TODO : может переименовать в layout.utils ?
export module iuic.core:layout;
import std;
import iuic.underlying;
import :layout.def;
import :text.token;
import :text.buff;
import :text.present;
import :text.fontset;
import :kernel;

namespace iuic {

struct layout_utils_base {
  layout_utils_base(kernel::hardware &kernel_, kernel::element e_) noexcept
      : kernel{kernel_}, element{e_} {};
  // Обычное сообщение для отладки
  void log(std::string_view message) const noexcept;
  // Предупреждение об исключительной ситвации.
  void warning(std::string_view message) const noexcept;
  // Сообщение об ошибке.
  void error(std::string_view message) const noexcept;

  // получение ссылки на собственный стиль
  style::cref self_style() const;

  style::cref style_of(const kernel::request &el) const;

  // получение ссылки на родительский стиль
  style::cref parent_style() const;

  // получение ссылки на viewport стиль
  style::cref root_style() const;

  units::ui::size root_size() const;

  units::upixel rem(units::rem) const noexcept;

  units::upixel vh(units::vh) const noexcept;

  units::upixel vw(units::vw) const noexcept;

  // отложить
  void defer();

  void discard(const kernel::request &);

protected: // общие нужды
  kernel::hardware &kernel;
  kernel::element element;

private: // реализация базовых концепций логирования
};

// Структура которая помогает
// при вычислении собственной позиции
struct frame_measure_utils : layout_utils_base {

  frame_measure_utils(kernel::hardware &kernel, kernel::element e_) noexcept;

  std::unique_ptr<utils::virtual_iterator<const kernel::request>>
  get_requests();

  enum err_r { AUTO, PERCENT, ERR };

  // TODO : Rename
  std::expected<units::upixel, err_r>
  upixel_of(const units::ui::adaptive_unit &utils) const noexcept {
    return std::visit(
        [this](auto &value) -> std::expected<units::upixel, err_r> {
          using type = std::remove_cvref_t<decltype(value)>;
          if constexpr (std::same_as<type, units::upixel>) {
            return value;
          } else if constexpr (std::same_as<type, units::percent>) {
            return std::unexpected{err_r::PERCENT};
          } else if constexpr (std::same_as<type, units::vw>) {
            return vw(value);
          } else if constexpr (std::same_as<type, units::vh>) {
            return vh(value);
          } else if constexpr (std::same_as<type, units::rem>) {
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
  frame_arrange_utils(kernel::hardware &kernel_hardware_,
                      kernel::element e_) noexcept
      : layout_utils_base{kernel_hardware_, e_} {};

  units::ui::rect self_area() const;

  std::unique_ptr<utils::virtual_iterator<const kernel::request>>
  get_requests();

  void apply(const kernel::request &, units::ui::rect);

  void apply(const kernel::request &, units::ui::rect, units::ui::rect);

  enum class side_e { WIDTH, HEIGHT };

  units::upixel
  width_upixel_of(const units::ui::adaptive_unit &unit) const noexcept {
    return std::visit(
        [this](auto &value) -> units::upixel {
          using type = std::remove_cvref_t<decltype(value)>;

          if constexpr (std::same_as<type, units::upixel>) {
            return value;
          } else if constexpr (std::same_as<type, units::percent>) {
            return self_area().size.w * value;
          } else if constexpr (std::same_as<type, units::vh>) {
            return vh(value);
          } else if constexpr (std::same_as<type, units::vw>) {
            return vw(value);
          } else if constexpr (std::same_as<type, units::rem>) {
            return rem(value);
          } else {
            return 0;
          }
        },
        unit);
  }

  units::upixel
  height_upixel_of(const units::ui::adaptive_unit &value_) const noexcept {
    return std::visit(
        [this](auto &val) -> units::upixel {
          using type = std::remove_cvref_t<decltype(val)>;

          if constexpr (std::same_as<type, units::upixel>) {
            return val;
          } else if constexpr (std::same_as<type, units::percent>) {
            return self_area().size.h * val;
          } else if constexpr (std::same_as<type, units::vh>) {
            return vh(val);
          } else if constexpr (std::same_as<type, units::vw>) {
            return vw(val);
          } else if constexpr (std::same_as<type, units::rem>) {
            return rem(val);
          } else {
            return 0;
          }
        },
        value_);
  }
};

struct text_measure_utils : layout_utils_base {
  text_measure_utils(kernel::hardware &kernel_hardware_, kernel::element e_,
                     const text::token::sequence &sq_,
                     const text::fontslot &font_)
      : layout_utils_base{kernel_hardware_, e_}, sq{sq_}, font{font_} {};

  text::token::sequence get_tokens() const;

  const text::glyph::atlas &get_atlas(style::font::cref);

private:
  text::token::sequence sq;
  const text::fontslot &font;
};

struct text_arrange_utils : layout_utils_base {
  text_arrange_utils(kernel::hardware &kernel_hardware_, kernel::element e_,
                     text::token::sequence sq_, const text::fontslot &font_)
      : layout_utils_base{kernel_hardware_, e_}, sq{sq_}, font{font_} {}

  units::ui::rect self_rect() const noexcept;

  units::ui::position self_position() const noexcept;

  const text::glyph::atlas &get_atlas(style::font::cref);

  text::token::sequence get_tokens() const;

  text::glyph::sequence capture_glyphs(text::glyph::sequence sq) const;
  // make present ...
private:
  text::token::sequence sq;
  const text::fontslot &font;
  static constexpr std::string_view err_token{"err..."};
};
} // namespace iuic
