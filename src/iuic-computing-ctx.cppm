

module;

#include <cstdint>
#include <exception>
#include <expected>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:computing_context;
import :base;
import :layout.def;

namespace iuic {

struct violated_computing_order : std::runtime_error {
  violated_computing_order()
      : std::runtime_error{
            "The order of evaluation of iuic::text_layout or "
            "iuic::frame_layout within iuic::computing_context for "
            "iuic::celement is broken. This is a core library bug. If you see "
            "this message, your release of the library is broken."} {}
};

struct computing_context;

// Дле представления используется
// Альтернативная блочная модель
struct celement {
  enum struct attribute_tags_t : std::uint8_t {
    null = 0,
    discarded = 1 << 1,
    text = 1 << 2,
    root = 1 << 3,
  } attribute_tags;
  enum struct stage_t : std::uint8_t {
    measure = 1,
    arrange,
    position, // text_layout skip this stage
    complite,
    undefined,
  } stage{stage_t::measure};
  enum struct error_tags_t : std::uint8_t {
    null = 0,
    invalid = 1 << 1,
    broken = 1 << 2,
  } error_tags;

  // область которую занимает элемент
  // Разрешаю сам себе вплоть до 24-28
  union {
    request_size area_request;
    ui_size applyed_size;
    ui_rect full_area;
    // err_handler
    // text_request
  };
};

constexpr celement::attribute_tags_t operator+(celement::attribute_tags_t lhs,
                                               celement::attribute_tags_t rhs) {
  return static_cast<celement::attribute_tags_t>(
      static_cast<std::underlying_type_t<celement::attribute_tags_t>>(lhs) |
      static_cast<std::underlying_type_t<celement::attribute_tags_t>>(rhs));
}

constexpr celement::attribute_tags_t &
operator+=(celement::attribute_tags_t &lhs, celement::attribute_tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr celement::attribute_tags_t operator-(celement::attribute_tags_t lhs,
                                               celement::attribute_tags_t rhs) {

  return static_cast<celement::attribute_tags_t>(
      static_cast<std::underlying_type_t<celement::attribute_tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<celement::attribute_tags_t>>(rhs));
}

constexpr celement::attribute_tags_t &
operator-=(celement::attribute_tags_t &lhs, celement::attribute_tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(celement::attribute_tags_t lhs,
                         celement::attribute_tags_t rhs) {

  return static_cast<celement::attribute_tags_t>(
             static_cast<std::underlying_type_t<celement::attribute_tags_t>>(
                 lhs) &
             static_cast<std::underlying_type_t<celement::attribute_tags_t>>(
                 rhs)) != celement::attribute_tags_t::null;
}

constexpr celement::error_tags_t operator+(celement::error_tags_t lhs,
                                           celement::error_tags_t rhs) {
  return static_cast<celement::error_tags_t>(
      static_cast<std::underlying_type_t<celement::error_tags_t>>(lhs) |
      static_cast<std::underlying_type_t<celement::error_tags_t>>(rhs));
}

constexpr celement::error_tags_t &operator+=(celement::error_tags_t &lhs,
                                             celement::error_tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr celement::error_tags_t operator-(celement::error_tags_t lhs,
                                           celement::error_tags_t rhs) {

  return static_cast<celement::error_tags_t>(
      static_cast<std::underlying_type_t<celement::error_tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<celement::error_tags_t>>(rhs));
}

constexpr celement::error_tags_t &operator-=(celement::error_tags_t &lhs,
                                             celement::error_tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(celement::error_tags_t lhs,
                         celement::error_tags_t rhs) {

  return static_cast<celement::error_tags_t>(
             static_cast<std::underlying_type_t<celement::error_tags_t>>(lhs) &
             static_cast<std::underlying_type_t<celement::error_tags_t>>(
                 rhs)) != celement::error_tags_t::null;
}

// Это интерфейс отвечает за возможность
// взаимодействия с иерархией
struct computing_hierarchy {
  virtual ~computing_hierarchy() = default;

  virtual computing_context *get_root(computing_context *) = 0;

  virtual computing_context *get_parent(computing_context *) = 0;

  virtual std::vector<computing_context *> get_childs(computing_context *) = 0;

  virtual void update_context_state(computing_context *) = 0;

  virtual computing_context *get_context_by_id(size_t) = 0;
};

// а еще хочеться нормальные интерфейс
// а еще хочеться чтобы была попытка соблюдения SRP
struct computing_context {
  friend class FCTree;

  // конструктор для вычисления фрейма
  computing_context(const frame_layout *layout_, const style *style_,
                    computing_hierarchy *hierarchy_, size_t parent_)
      : frame_layout{layout_}, style{style_}, hierarhy{hierarchy_},
        parent{parent_}, brother{parent_} {
    if (not hierarhy) {
      throw std::runtime_error{"Null hierarchy"};
    }
  };

  // конструктор для вычисления текста
  computing_context(const text_layout *layout_, const style *style_,
                    computing_hierarchy *hierarchy_, size_t parent_)
      : text_layout{layout_}, style{style_}, hierarhy{hierarchy_},
        parent{parent_}, brother{parent_} {
    element.attribute_tags += celement::attribute_tags_t::text;
    if (not hierarhy) {
      throw std::runtime_error{"Null hierarchy"};
    }
  };

public: // hierarchy
  void set_brother(size_t id);

  void unset_brother();

  size_t get_parent_id() const;

  size_t get_brother_id() const;

  computing_context *get_parent();

  std::vector<computing_context *> get_childs();

public: // get's
  const style &get_style() const noexcept;

  std::variant<const frame_layout *, const text_layout *>
  get_layout() const noexcept;

  std::expected<request_size, celement::stage_t> get_request() const noexcept;

  std::expected<ui_size, celement::stage_t> get_size() const noexcept;

  std::expected<ui_rect, celement::stage_t> get_rect() const noexcept;

public: // property's
  bool is_discarted() const noexcept;

public: // modify
  // first stage
  void apply(request_size);

  // second stage
  void apply(ui_size);

  // third stage
  void apply(ui_position);

  void discard();

private:
  union {
    const frame_layout *frame_layout;
    const text_layout *text_layout;
  };
  const style *style{nullptr};
  computing_hierarchy *hierarhy{nullptr};
  size_t parent{std::numeric_limits<size_t>::max()};
  size_t brother{
      parent}; // ссылка на брата. Если равно parent, то элемент последний.
  celement element{};
};
}; // namespace iuic
