

module;

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

export module iuic.core:computing_context;
import :base;

namespace iuic {

// Контекст каждого элемента
// используемый для вычисленией.
struct computing_context;

// Дле представления используется
// Альтернативная блочная модель
struct celement {
  enum struct state_tags_t : std::uint8_t {
    null = 0,
    discarded = 1 << 1,
    root = 1 << 4,
  } state_tags;
  enum struct area_tags_t : std::uint8_t {
    null = 0,
    area_request_dispatched = 1 << 0,
    area_request_strong_appyed = 1 << 2,
    area_request_soft_appyed = 1 << 2, // rename
    area_request_deferred = 1 << 3,
  } area_tags;
  enum struct position_tags_t : std::uint8_t {
    null = 0,
    set_position_is_applyed = 1 << 0,
    set_position_is_deferred = 1 << 1,
    position_type_absolute = 1 << 2,
  } position_tags;
  enum struct error_tags_t : std::uint8_t {
    null = 0,
    invalid = 1 << 1,
    broken = 1 << 2,
  } error_tags;

  // область которую занимает элемент
  ui_rect full_area{};
};

constexpr celement::state_tags_t operator+(celement::state_tags_t lhs,
                                           celement::state_tags_t rhs) {
  return static_cast<celement::state_tags_t>(
      static_cast<std::underlying_type_t<celement::state_tags_t>>(lhs) |
      static_cast<std::underlying_type_t<celement::state_tags_t>>(rhs));
}

constexpr celement::state_tags_t &operator+=(celement::state_tags_t &lhs,
                                             celement::state_tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr celement::state_tags_t operator-(celement::state_tags_t lhs,
                                           celement::state_tags_t rhs) {

  return static_cast<celement::state_tags_t>(
      static_cast<std::underlying_type_t<celement::state_tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<celement::state_tags_t>>(rhs));
}

constexpr celement::state_tags_t &operator-=(celement::state_tags_t &lhs,
                                             celement::state_tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(celement::state_tags_t lhs,
                         celement::state_tags_t rhs) {

  return static_cast<celement::state_tags_t>(
             static_cast<std::underlying_type_t<celement::state_tags_t>>(lhs) &
             static_cast<std::underlying_type_t<celement::state_tags_t>>(
                 rhs)) != celement::state_tags_t::null;
}

constexpr celement::area_tags_t operator+(celement::area_tags_t lhs,
                                          celement::area_tags_t rhs) {
  return static_cast<celement::area_tags_t>(
      static_cast<std::underlying_type_t<celement::area_tags_t>>(lhs) |
      static_cast<std::underlying_type_t<celement::area_tags_t>>(rhs));
}

constexpr celement::area_tags_t &operator+=(celement::area_tags_t &lhs,
                                            celement::area_tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr celement::area_tags_t operator-(celement::area_tags_t lhs,
                                          celement::area_tags_t rhs) {

  return static_cast<celement::area_tags_t>(
      static_cast<std::underlying_type_t<celement::area_tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<celement::area_tags_t>>(rhs));
}

constexpr celement::area_tags_t &operator-=(celement::area_tags_t &lhs,
                                            celement::area_tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(celement::area_tags_t lhs, celement::area_tags_t rhs) {

  return static_cast<celement::area_tags_t>(
             static_cast<std::underlying_type_t<celement::area_tags_t>>(lhs) &
             static_cast<std::underlying_type_t<celement::area_tags_t>>(rhs)) !=
         celement::area_tags_t::null;
}

constexpr celement::position_tags_t operator+(celement::position_tags_t lhs,
                                              celement::position_tags_t rhs) {
  return static_cast<celement::position_tags_t>(
      static_cast<std::underlying_type_t<celement::position_tags_t>>(lhs) |
      static_cast<std::underlying_type_t<celement::position_tags_t>>(rhs));
}

constexpr celement::position_tags_t &operator+=(celement::position_tags_t &lhs,
                                                celement::position_tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr celement::position_tags_t operator-(celement::position_tags_t lhs,
                                              celement::position_tags_t rhs) {

  return static_cast<celement::position_tags_t>(
      static_cast<std::underlying_type_t<celement::position_tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<celement::position_tags_t>>(rhs));
}

constexpr celement::position_tags_t &operator-=(celement::position_tags_t &lhs,
                                                celement::position_tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(celement::position_tags_t lhs,
                         celement::position_tags_t rhs) {

  return static_cast<celement::position_tags_t>(
             static_cast<std::underlying_type_t<celement::position_tags_t>>(
                 lhs) &
             static_cast<std::underlying_type_t<celement::position_tags_t>>(
                 rhs)) != celement::position_tags_t::null;
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
  computing_context(const layout *layout_, const style *style_,
                    computing_hierarchy *hierarchy_, size_t parent_)
      : layout{layout_}, style{style_}, hierarhy{hierarchy_}, parent{parent_},
        brother{parent_} {
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

  const layout &get_layout() const noexcept;

  // TODO : Rename
  const ui_rect &get_rect() const noexcept;

public: // property's
  bool is_discarted() const noexcept;

  bool is_area_request_dispatched() const noexcept;

public: // modify
  // для установки площади всегда должна быть причина
  // можно переработать
  void set_area(ui_size, celement::area_tags_t);

  // для установки позиции всегда должна быть причина
  void set_position(ui_position, celement::position_tags_t);

  void discard();

private:
  const layout *layout{nullptr};
  const style *style{nullptr};
  computing_hierarchy *hierarhy{nullptr};
  size_t parent{std::numeric_limits<size_t>::max()};
  size_t brother{
      parent}; // ссылка на брата. Если равно parent, то элемент последний.
  celement element{};
};
}; // namespace iuic
