

module;

#include <cstdint>
#include <exception>
#include <expected>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:computing.context;
import :base;
import :layout.def;
import :policy;

// forward decl
namespace iuic::text {
struct present;
};

namespace iuic::computing {

struct violated_order : std::runtime_error {
  violated_order()
      : std::runtime_error{
            "The computing order of iuic::text_layout or "
            "iuic::frame_layout within iuic::computing_context for "
            "iuic::celement is broken. This is a core library bug. If you see "
            "this message, your release of the library is broken."} {}
};

struct context;

// Дле представления используется
// Альтернативная блочная модель
struct element {
  enum struct attribute_tags_t : std::uint8_t {
    null = 0,
    discarded = 1 << 1,
    text = 1 << 2,
    root = 1 << 3,
  } attribute_tags;
  enum struct stage_t : std::uint8_t {
    measure = 1,
    frame_arrange,
    frame_position, // text_layout skip this stage
    text_arrange,
    text_position,
    complite,
    undefined,
  } stage{stage_t::measure};

  // область которую занимает элемент
  // Разрешаю сам себе вплоть до 24-28
  union {
    request_size area_request;
    ui_size applyed_size;
    ui_rect full_area;
    // err_handler
    // text_request
  };
  union {
    const frame_layout *frame_layout;
    const text_layout *text_layout;
  };
};

constexpr element::attribute_tags_t operator+(element::attribute_tags_t lhs,
                                              element::attribute_tags_t rhs) {
  return static_cast<element::attribute_tags_t>(
      static_cast<std::underlying_type_t<element::attribute_tags_t>>(lhs) |
      static_cast<std::underlying_type_t<element::attribute_tags_t>>(rhs));
}

constexpr element::attribute_tags_t &operator+=(element::attribute_tags_t &lhs,
                                                element::attribute_tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr element::attribute_tags_t operator-(element::attribute_tags_t lhs,
                                              element::attribute_tags_t rhs) {

  return static_cast<element::attribute_tags_t>(
      static_cast<std::underlying_type_t<element::attribute_tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<element::attribute_tags_t>>(rhs));
}

constexpr element::attribute_tags_t &operator-=(element::attribute_tags_t &lhs,
                                                element::attribute_tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(element::attribute_tags_t lhs,
                         element::attribute_tags_t rhs) {

  return static_cast<element::attribute_tags_t>(
             static_cast<std::underlying_type_t<element::attribute_tags_t>>(
                 lhs) &
             static_cast<std::underlying_type_t<element::attribute_tags_t>>(
                 rhs)) != element::attribute_tags_t::null;
}

// Это интерфейс отвечает за возможность
// взаимодействия с иерархией
struct computing_hierarchy {
  virtual ~computing_hierarchy() = default;

  virtual context *get_root(context *) = 0;

  virtual context *get_parent(context *) = 0;

  virtual std::vector<context *> get_childs(context *) = 0;

  virtual void update_context_state(context *) = 0;

  virtual context *get_context_by_id(size_t) = 0;

  virtual size_t get_id(context *) = 0;
};

// а еще хочеться нормальные интерфейс
// а еще хочеться чтобы была попытка соблюдения SRP
struct context {
  struct hierarchy_t {
    computing_hierarchy *interface{nullptr};
    size_t parent{std::numeric_limits<size_t>::max()};
    size_t brother{
        parent}; // ссылка на брата. Если равно parent, то элемент последний.
  };

  struct info_t {
    style::ref style;
    uid_t uid;
    z_order_t order;
    policy::hovered hovered_p{policy::hovered::none};
    policy::event event_p;
  };
  friend class tree; // TODO : end of friend

  // конструктор для вычисления фрейма
  context(const frame_layout *layout_, style::ref style_,
          computing_hierarchy *hierarchy_, size_t parent_)
      : hierarchy{hierarchy_, parent_}, info{style_} {
    element.frame_layout = layout_;
    if (not hierarchy_) {
      throw std::runtime_error{"Null hierarchy"};
    }
  };

  // конструктор для вычисления текста
  context(const text_layout *layout_, style::ref style_,
          computing_hierarchy *hierarchy_, size_t parent_)
      : hierarchy{hierarchy_, parent_}, info{style_} {
    element.attribute_tags += element::attribute_tags_t::text;
    element.text_layout = layout_;
    if (not hierarchy_) {
      throw std::runtime_error{"Null hierarchy"};
    }
  };

public: // get's ordered
  std::expected<request_size, element::stage_t> get_request() const noexcept;

  std::expected<ui_size, element::stage_t> get_size() const noexcept;

  std::expected<ui_rect, element::stage_t> get_rect() const noexcept;

public: // get's free
  std::variant<const frame_layout *, const text_layout *>
  get_layout() const noexcept;

  info_t &get_info();

  const info_t &get_info() const;

  hierarchy_t &get_hierarchy();

  const hierarchy_t &get_hierarchy() const;

public:                               // setters ordered
  bool is_discarted() const noexcept; // ???

public: // modify
  // first stage
  void apply(request_size);

  // only for frame
  void apply(ui_size);

  // third stage
  void apply(ui_position);

  void discard();

private:
  element element{};
  hierarchy_t hierarchy;
  info_t info;
};

}; // namespace iuic::computing
