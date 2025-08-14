
module;

#include <cstddef>
#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <print>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

export module iuic.core:ftc;
import :base;
import :layout.box;

namespace iuic {

// WARNING : Base and wrong impl
// TODO : write normal root layout
struct root_element_layout : public layout {
  void self_size(area_utils utils) const noexcept override {
    auto &style = utils.self_style();
    utils.set_hard_size(style.shape.max_size);

    auto requests = utils.get_requests();

    for (auto &&rq : requests) {
      rq.apply();
    }

    std::println("Root size : w={},h={}", style.shape.max_size.w,
                 style.shape.max_size.h);
  };

  void set_childs_position(position_utils utils) const noexcept override {
    auto position = ui_position{0, 0};

    auto def = position;

    auto content = utils.content();

    for (auto &&rq : content) {
      auto &style = rq.style_of();

      position.y += style.shape.margin.top;
      position.x += style.shape.margin.left;
      rq.apply(position);
      auto &size = rq.size_of();
      position.y += size.h;
      position.x = def.x;

      std::println("Request at root : {}:{}", size.h, size.w);
    }
  };

  void balancing(balancing_utils) const noexcept override{
      // ake box layout
  };
};

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

// Возможно сделаю его приватным
struct computing_hierarchy {
  virtual ~computing_hierarchy() = default;

  virtual computing_context *get_root(computing_context *) = 0;

  virtual computing_context *get_parent(computing_context *) = 0;

  virtual std::vector<computing_context *> get_childs(computing_context *) = 0;

  virtual void update_context_state(computing_context *) = 0;

  virtual computing_context *get_context_by_id(size_t) = 0;
};

// хочеться сократить размер со 180 до <120
// а еще хочеться нормальные интерфейс
// а еще хочеться чтобы была попытка соблюдения SRP
struct computing_context {
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

// с computing_context
// Так как вычислительное дерево одно на контекст
// в целом его можно сделать пожирнее,
// но исключительно в рамках НУЖНЫХ инструментов

// Возможно частично инвертировать и свести зависимости
// (Упровление сявзями и вычислениями) computing_context -> computing_core <-
// FCTree (Упровлени иерархией)
// Вынисти root элемен и его управление из context

struct FCTree final : private computing_hierarchy {
public: // base proxy struct's
  struct sentinel;
  struct range_based_for_proxy;
  struct const_range_based_for_proxy;
  struct reverse_range_based_for_proxy;
  struct const_reverse_range_based_for_proxy;

public: // BIG-VI
  ~FCTree() = default;
  FCTree() = default;
  FCTree(const FCTree &) = delete;
  FCTree &operator=(const FCTree &) = delete;
  FCTree(FCTree &&) = delete;
  FCTree &operator=(FCTree &&) = delete;

public: // Public Interface
  /*
    Отчистка девева.
  */
  void reset();

  /*
    Добовление нового элемента и вход в его контекст.
  */
  void add(const style &style, const layout *layout);

  /*
    Выход из контекста родительского элемента.
  */
  void up();

  /*
    Получение последнего элемента.
  */
  computing_context &last();

  /*
   for(auto&& cc : ctree.range_for()) { ... }.
   Перебо всего дерева от начала до конца.
  */
  range_based_for_proxy range_for();

  /*
    for(auto&& cc : ctree.range_for()) { ... }.
    Перебо всего дерева от начала до конца.
  */
  const_range_based_for_proxy range_for() const;

  /*
   for(auto&& cc : ctree.reverse_range_for()) { ... }.
   Перебо всего дерева от конца к началу.
  */
  reverse_range_based_for_proxy reverse_range_for();

  /*
   for(auto&& cc : ctree.reverse_range_for()) { ... }.
   Перебо всего дерева от конца к началу.
  */
  const_reverse_range_based_for_proxy reverse_range_for() const;

  /*
    Вывести текущее дерево с его состоянием в std::cout.
  */
  void print_tree() const noexcept;

public: // root style pubic interface
  void set_root_size(ui_size);

  computing_context &get_root();

private: // Private Hierarhy Interface
  computing_context *get_parent(computing_context *ctx) override {
    if (ctx->get_parent_id() == std::numeric_limits<size_t>::max()) {
      return &root.ctx;
    }
    return &nodes[ctx->get_parent_id()];
  };

  std::vector<computing_context *> get_childs(computing_context *ctx) override {
    // WARNING : Очень хрупко
    // Требуеться рефакторинг
    std::vector<computing_context *> res{};

    if (nodes.empty()) {
      return res;
    }

    auto current = ctx == &root.ctx ? &nodes[0] : ctx + 1;

    if (current == &nodes.back() + 1 || current->get_parent() != ctx) {
      return res;
    }

    for (;;) {
      if (not current->is_discarted()) {
        res.push_back(current);
      }

      if (current->get_parent_id() == current->get_brother_id()) {
        break;
      }

      current = &nodes[current->get_brother_id()];
    }

    return res;
  };

  computing_context *get_root(computing_context *) override {
    return &root.ctx;
  };

  void update_context_state(computing_context *) override {
    // ...
  };

  computing_context *get_context_by_id(size_t id) override {
    return &nodes[id];
  };

private: // Data
  struct root_t {
    static constexpr auto id = std::numeric_limits<size_t>::max();
    root_t(FCTree *ctree)
        : style{}, last_child{},
          ctx{&layout::instance<root_element_layout>(), &style, ctree, id} {}
    style style;           // можно унифицировать стиль
    size_t last_child;     // помошник в построении макета
    computing_context ctx; // сам контекст
  } root{this};
  std::vector<computing_context> nodes;
  // parent\last_brather
  std::stack<std::pair<size_t, size_t>> parent{};
};

struct FCTree::sentinel final {};

// TODO : Сделать все proxy no move\copy
// и в деструкторе вызывать метод FCT который
// работает с тегами
struct FCTree::range_based_for_proxy {
  range_based_for_proxy(FCTree &);
  struct iterator {
    iterator &operator++() {
      ++ptr;
      return *this;
    };

    iterator(computing_context *ptr_, void *end_) : ptr{ptr_}, end{end_} {}

    // должен игнорировать discarted элементы
    iterator operator++(int) {
      auto tmp = *this;
      ++ptr;
      return tmp;
    };

    computing_context &operator*() { return *ptr; };

    bool is_valide() { return ptr != nullptr && ptr < end; };

  private:
    computing_context *ptr;
    void *end;
  };

  iterator begin() { return begin_; };

  sentinel end() { return {}; };

private:
  iterator begin_;
};

struct FCTree::const_range_based_for_proxy {
  const_range_based_for_proxy(const FCTree &);

  struct const_iterator {
    const_iterator &operator++() {
      ++ptr;
      return *this;
    };

    const_iterator(const computing_context *ptr_, const void *end_)
        : ptr{ptr_}, end{end_} {}

    // должен игнорировать discarted элементы
    const_iterator operator++(int) {
      auto tmp = *this;
      ++ptr;
      return tmp;
    };

    const computing_context &operator*() { return *ptr; };

    bool is_valide() { return ptr != nullptr && ptr < end; };

  private:
    const computing_context *ptr;
    const void *end;
  };

  const_iterator begin() { return begin_; };

  sentinel end() { return {}; };

private:
  const_iterator begin_;
};

struct FCTree::reverse_range_based_for_proxy {

  reverse_range_based_for_proxy(FCTree &);
  struct reverse_iterator {
    reverse_iterator &operator++() {
      --ptr;
      return *this;
    };

    reverse_iterator(computing_context *ptr_, void *end_)
        : ptr{ptr_}, end{end_} {}

    // должен игнорировать discarted элементы
    reverse_iterator operator++(int) {
      auto tmp = *this;
      --ptr;
      return tmp;
    };

    computing_context &operator*() { return *ptr; };

    bool is_valide() { return ptr != nullptr && ptr > end; };

  private:
    computing_context *ptr;
    void *end;
  };

  reverse_iterator begin() { return begin_; };

  sentinel end() { return {}; };

private:
  reverse_iterator begin_;
};

struct FCTree::const_reverse_range_based_for_proxy {
  const_reverse_range_based_for_proxy(const FCTree &);
  struct const_reverse_iterator {
    const_reverse_iterator &operator++() {
      --ptr;
      return *this;
    };

    const_reverse_iterator(const computing_context *ptr_, const void *end_)
        : ptr{ptr_}, end{end_} {}

    // должен игнорировать discarted элементы
    const_reverse_iterator operator++(int) {
      auto tmp = *this;
      --ptr;
      return tmp;
    };

    const computing_context &operator*() { return *ptr; };

    bool is_valide() { return ptr != nullptr && ptr > end; };

  private:
    const computing_context *ptr;
    const void *end;
  };

  const_reverse_iterator begin() { return begin_; };

  sentinel end() { return {}; };

private:
  const_reverse_iterator begin_;
};

constexpr bool operator==(FCTree::range_based_for_proxy::iterator &it,
                          FCTree::sentinel &s) {
  return not it.is_valide();
};

constexpr bool operator!=(FCTree::range_based_for_proxy::iterator &it,
                          FCTree::sentinel &s) {
  return it.is_valide();
};

constexpr bool
operator==(FCTree::reverse_range_based_for_proxy::reverse_iterator &it,
           FCTree::sentinel &s) {
  return not it.is_valide();
};

constexpr bool
operator!=(FCTree::reverse_range_based_for_proxy::reverse_iterator &it,
           FCTree::sentinel &s) {
  return it.is_valide();
};

}; // namespace iuic
