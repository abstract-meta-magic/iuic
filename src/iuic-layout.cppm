

module;

#include <expected>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
// TODO : может переименовать в layout.utils ?
export module iuic.core:layout;
import :base;
import :layout.def;
import :computing_context;

namespace iuic {

struct layout_utils_base {
  layout_utils_base(computing_context *ctx_) noexcept : ctx{ctx_} {};
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
  const style &root_style() const;

  pixel_t rem_width(rem_t) const;

  pixel_t rem_hieght(rem_t) const;

  pixel_t vh(vh_t) const;

  pixel_t vw(vw_t) const;

  // отложить
  void defer();

protected: // общие нужды
  computing_context *ctx{nullptr};

private: // реализация базовых концепций логирования
};

struct measure_child_request {

  constexpr measure_child_request(computing_context *of_) : of{of_} {}

  request_size value() const noexcept;

  const style &style_of() const noexcept;

private:
  computing_context *of;
};

// Структура которая помогает
// при вычислении собственной позиции
struct frame_measure_utils : layout_utils_base {

  frame_measure_utils(computing_context *self) noexcept;

  std::vector<measure_child_request> get_requests();
};

struct area_request {
  constexpr area_request(computing_context *of_) noexcept : of{of_} {}

  constexpr area_request(const area_request &) = default;

  // применить измененное решение
  void apply(ui_size);

  // выкинуть элемент = игнорировать его
  void discard();

  const style &style_of() const;

  request_size value() const noexcept;

private:
  computing_context *of;
};
// Набор команд и свойс
// для точного определения позиций
// и размеров
struct frame_arrange_utils : layout_utils_base {
  frame_arrange_utils(computing_context *ctx_) noexcept
      : layout_utils_base{ctx_} {};

  ui_size get_size() const noexcept;

  std::vector<area_request> get_requests();

  // может еще обрезку тут делать ...
};

struct position_request {
  constexpr position_request(computing_context *owner_) noexcept
      : owner{owner_} {}

  void apply(ui_position);

  void discard();

  const style &style_of() const noexcept;

  ui_size size_of() const noexcept;

private:
  computing_context *owner;
};

struct frame_position_utils : layout_utils_base {
  frame_position_utils(computing_context *ctx_) noexcept
      : layout_utils_base{ctx_} {};

  ui_position self_position() const noexcept;

  std::vector<position_request> content();

  // тут могут быть статические методы для
  // помощи в вычислении позиций
};

// In version 0.2
enum balancing_result {
  SUCCES,
  RE_ALL,
  RE_PARENT,
  RE_CHILDS,
};

} // namespace iuic
