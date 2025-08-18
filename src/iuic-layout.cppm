

module;

#include <string_view>
#include <type_traits>
#include <vector>
export module iuic.core:layout;
import :base;

namespace iuic {
template <typename T>
concept layout_cpt = std::is_base_of_v<layout, T>;

struct area_request {
  constexpr area_request(computing_context *of_) noexcept : of{of_} {}

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
  computing_context *of;
};

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

  // отложить
  void defer();

protected: // общие нужды
  computing_context *ctx{nullptr};

private: // реализация базовых концепций логирования
};

// Структура которая помогает
// при вычислении собственной позиции
struct area_utils : layout_utils_base {

  area_utils(computing_context *self) noexcept;

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

  // терминальный метод.
  // жестко задать размер,
  // без запроса к родительскому элименту.
  // Может привести к некоторым визуальным багам или
  // к пометке элемента как discarded.
  void set_hard_size(ui_size);

  // запросы на выделение площади от дочерних объектов
  std::vector<area_request> get_requests();

  // терминальный метод.
  // помечает элемент и его детей как discarted
  void self_discard();
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
  position_utils(computing_context *ctx_) noexcept : layout_utils_base{ctx_} {};
  ;

  const ui_position &self_position() const noexcept;

  std::vector<position_request> content();
};

// Набор команд и свойс
// для точного определения позиций
// и размеров
struct balancing_utils : layout_utils_base {
  balancing_utils(computing_context *ctx_) noexcept
      : layout_utils_base{ctx_} {};
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

export struct layout {
  // measure && layout

  virtual ~layout() = default;

  // примитивная оценка собственного размера
  // можно подумать о предоставлении ограничителя на
  // вычисления размеров относительно родителя
  // process area
  virtual void self_size(area_utils) const noexcept = 0;
  // приблезительное расположение элементов
  // process position
  virtual void set_childs_position(position_utils) const noexcept = 0;

  // Балансировка очень сложна
  // тут сложно и нужно подумать
  // Сверху приходит ваш ui_rect, а вы должны
  // максимально точно вычислить ui_rect своих дитей
  // может быть вызван более одного раза
  // process balancing
  virtual void balancing(balancing_utils) const noexcept = 0;

  template <layout_cpt T> static constexpr const layout &instance() {
    static constexpr T _{};
    return _;
  };
};
} // namespace iuic
