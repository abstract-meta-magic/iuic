
module;

#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <stack>
#include <type_traits>
#include <vector>

export module iuic.core:ftc;
import :base;
import :layout.box;

namespace iuic {

// Дле представления используется
// Альтернативная блочная модель
struct celement {
  enum struct tags_t : std::uint8_t {
    null_state = 0,
    size_request_dispatched = 1 << 0,
    request_strong_applyed = 1 << 1,
    position_inspected = 1 << 2,
    discarded = 1 << 3,
    invalid = 1 << 4,
    broken = 1 << 5,
    absolute = 1 << 6,
    // viewport
  } tags;
  using enum tags_t;

  // область которую занимает элемент
  ui_rect full_area{};
};

constexpr celement::tags_t operator+(celement::tags_t lhs,
                                     celement::tags_t rhs) {
  return static_cast<celement::tags_t>(
      static_cast<std::underlying_type_t<celement::tags_t>>(lhs) |
      static_cast<std::underlying_type_t<celement::tags_t>>(rhs));
}

constexpr celement::tags_t &operator+=(celement::tags_t &lhs,
                                       celement::tags_t rhs) {
  lhs = lhs + rhs;
  return lhs;
}

constexpr celement::tags_t operator-(celement::tags_t lhs,
                                     celement::tags_t rhs) {

  return static_cast<celement::tags_t>(
      static_cast<std::underlying_type_t<celement::tags_t>>(lhs) &
      ~static_cast<std::underlying_type_t<celement::tags_t>>(rhs));
}
constexpr celement::tags_t &operator-=(celement::tags_t &lhs,
                                       celement::tags_t rhs) {
  lhs = lhs - rhs;
  return lhs;
}

constexpr bool operator&(celement::tags_t lhs, celement::tags_t rhs) {

  return static_cast<celement::tags_t>(
             static_cast<std::underlying_type_t<celement::tags_t>>(lhs) &
             static_cast<std::underlying_type_t<celement::tags_t>>(rhs)) !=
         celement::tags_t::null_state;
}

// хочеться сократить размер со 180 до <120
struct computing_context {
  const layout *layout{nullptr};
  const style *style{nullptr};
  celement element{};
  // ПЕРЕИМЕНОВАТЬ что-то вроде рабочей группы
  std::list<computing_context *> applyed_group;
  // позже будет замененно на pmr::vector
  std::stack<area_request, std::vector<area_request>> requests;
  size_t parent{std::numeric_limits<size_t>::max()};
};

// flat tree of calculations
struct FTC {
  void clear() {
    nodes.clear();
    parent = {}; // ...
  };

  void add(const style &style, const layout *layout) {
    static constexpr box_layout box{};
    nodes.push_back({layout, &style});

    if (not parent.empty()) {
      nodes.back().parent = parent.top();
    }

    parent.push(nodes.size() - 1);
  };

  computing_context &get_parent(computing_context &);

  size_t get_current_id() const {
    // REFACTOR THIS
    if (nodes.empty()) {
      return std::numeric_limits<size_t>::max();
    }

    return nodes.size() - 1;
  };

  void up() { parent.pop(); };

  std::vector<computing_context> &get();

private:
  computing_context root;
  std::vector<computing_context> nodes;
  std::stack<std::size_t> parent;
};
}; // namespace iuic
