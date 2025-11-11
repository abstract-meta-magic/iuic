

module;

#include <expected>
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
import :computing.context;

namespace iuic {

struct layout_utils_base {
  layout_utils_base(computing::context *ctx_) noexcept : ctx{ctx_} {};
  // Обычное сообщение для отладки
  void log(std::string_view message) const noexcept;
  // Предупреждение об исключительной ситвации.
  void warning(std::string_view message) const noexcept;
  // Сообщение об ошибке.
  void error(std::string_view message) const noexcept;

  // получение ссылки на собственный стиль
  style::cref self_style() const;

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

protected: // общие нужды
  computing::context *ctx{nullptr};

private: // реализация базовых концепций логирования
};

struct measure_child_request {

  constexpr measure_child_request(computing::context *of_) : of{of_} {}

  request_size value() const;

  style::cref style_of() const noexcept;

private:
  computing::context *of;
};

// Структура которая помогает
// при вычислении собственной позиции
struct frame_measure_utils : layout_utils_base {

  frame_measure_utils(computing::context *self) noexcept;

  std::vector<measure_child_request> get_requests();

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

export struct area_request {
  constexpr area_request(computing::context *of_) noexcept : of{of_} {}

  constexpr area_request(const area_request &) = default;

  // применить измененное решение
  void apply(ui_size);

  // выкинуть элемент = игнорировать его
  void discard();

  style::cref style_of() const;

  request_size value() const;

private:
  computing::context *of;
};
// Набор команд и свойс
// для точного определения позиций
// и размеров
struct frame_arrange_utils : layout_utils_base {
  frame_arrange_utils(computing::context *ctx_) noexcept
      : layout_utils_base{ctx_} {};

  ui_size self_size() const;

  std::vector<area_request> get_requests();

  enum class side_e { WIDTH, HEIGHT };

  upixel_t width_upixel_of(const ui_adaptive_unit &unit) const noexcept {
    return std::visit(
        [this](auto &value) -> upixel_t {
          using type = std::remove_cvref_t<decltype(value)>;

          if constexpr (std::same_as<type, upixel_t>) {
            return value;
          } else if constexpr (std::same_as<type, percent_t>) {
            return self_size().w * value;
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
            return self_size().h * val;
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

struct position_request {
  constexpr position_request(computing::context *owner_) noexcept
      : owner{owner_} {}

  void apply(ui_position);

  void discard();

  style::cref style_of() const noexcept;

  ui_size size_of() const;

private:
  computing::context *owner;
};

struct frame_position_utils : layout_utils_base {
  frame_position_utils(computing::context *ctx_) noexcept
      : layout_utils_base{ctx_} {};

  ui_position self_position() const;

  ui_size self_size() const;

  std::vector<position_request> content();

  // тут могут быть статические методы для
  // помощи в вычислении позиций
};

struct text_measure_utils : layout_utils_base {
  text_measure_utils(computing::context *ctx_, const text::token::sequence &sq_)
      : layout_utils_base{ctx_}, sq{sq_} {};

  const text::token::sequence &tokens() const;

private:
  const text::token::sequence &sq;
};

struct text_arrange_utils : layout_utils_base {
  text_arrange_utils(computing::context *ctx_, text::present &present_,
                     const text::token::sequence &sq_)
      : layout_utils_base{ctx_}, sq{sq_}, present{present_} {}

  ui_size self_size() const noexcept;

  void apply();

  // mb take present_node ???
  text::present &get_present();

  const text::token::sequence &get_tokens() const;
  // make present ...
private:
  text::present &present;
  const text::token::sequence &sq;
  static constexpr std::string_view err_token{"err..."};
};
} // namespace iuic
