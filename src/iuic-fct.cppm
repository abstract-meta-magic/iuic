
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
#include <variant>
#include <vector>

export module iuic.core:fct;
import :base;
import :layout;
import :computing_context;

namespace iuic {

/*
  Специайльный layout для корневого элемента.
 */
struct : public frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override {
    return {{percent_t{100}, percent_t{100}}};
  };

  void arrange(frame_arrange_utils utils) const noexcept override {
    auto self_size = utils.self_size();
    auto available_size = self_size;

    auto requests = utils.get_requests();

    // что мне нужно для вычисление
    // нужно не выйти за пределы от self_size
    for (auto &&rq : requests) {
      auto rq_value = rq.value();
      auto &min = rq.style_of().shape.min_size;

      bool valide{true};

      ui_size res = {.w = utils.width_upixel_of(rq_value.width),
                     .h = utils.height_upixel_of(rq_value.height)};

      auto style_min_width = utils.width_upixel_of(min.w);
      if (style_min_width > res.w) {
        res.w = style_min_width;
      }

      auto style_min_height = utils.height_upixel_of(min.h);
      if (style_min_height > res.h) {
        res.h = style_min_height;
      }

      rq.apply(res);
    }
  };

  static upixel_t margin_top(frame_position_utils &utils,
                             position_request &rq) noexcept {
    return std::visit(
        [&](auto &margin_top) -> upixel_t {
          using type = std::remove_cvref_t<decltype(margin_top)>;
          if constexpr (std::same_as<type, upixel_t>) {
            return margin_top;
          } else if constexpr (std::same_as<type, percent_t>) {
            return utils.self_size().h * margin_top;
          } else if constexpr (std::same_as<type, vh_t>) {
            return utils.root_size().h * margin_top;
          } else if constexpr (std::same_as<type, vh_t>) {
            return utils.root_size().w * margin_top;
          } else if constexpr (std::same_as<type, rem_t>) {
            return utils.rem(margin_top);
          } else {
            return {};
          }
        },
        rq.style_of().positioning.margin.top);
  };

  static upixel_t margin_left(frame_position_utils &utils,
                              position_request &rq) noexcept {
    return std::visit(
        [&](auto &margin_left) -> upixel_t {
          using type = std::remove_cvref_t<decltype(margin_left)>;
          if constexpr (std::same_as<type, upixel_t>) {
            return margin_left;
          } else if constexpr (std::same_as<type, ui_auto>) {
            return upixel_t{};
          } else if constexpr (std::same_as<type, percent_t>) {
            return utils.self_size().w * margin_left;
          } else if constexpr (std::same_as<type, vh_t>) {
            return utils.root_size().h * margin_left;
          } else if constexpr (std::same_as<type, vh_t>) {
            return utils.root_size().w * margin_left;
          } else if constexpr (std::same_as<type, rem_t>) {
            return utils.rem(margin_left);
          } else {
            return {};
          }
        },
        rq.style_of().positioning.margin.left);
  };
  void position(frame_position_utils utils) const noexcept override {
    auto current_pos = utils.self_position();

    auto content = utils.content();
    for (auto &&rq : content) {
      auto size = rq.size_of(); // ordered

      // fix me
      current_pos.y += margin_top(utils, rq);
      current_pos.x += margin_left(utils, rq);

      rq.apply(current_pos);

      current_pos.y += size.h;

      current_pos.x = 0;
    }
  };
} constexpr root_element_layout{};

// с computing_context
// Так как вычислительное дерево одно на контекст
// в целом его можно сделать пожирнее,
// но исключительно в рамках НУЖНЫХ инструментов
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
    Инвалидирует все итераторы и range_proxy.
  */
  void reset();

  /*
    Добовление нового элемента и вход в его контекст.
    Инвалидирует все итераторы и range_proxy.
  */
  void add(const style &style, const frame_layout *layout);

  void add(const style &style, const text_layout *layout);

  /*
    Выход из контекста родительского элемента.
  */
  void up();

  /*
    Получение последнего элемента.
  */
  computing_context &last() noexcept;

  /*
    Получение последнего элемента.
    Может возвращать корневой элемент.
  */
  const computing_context &last() const noexcept;

  computing_context &current() noexcept;

  const computing_context &current() const noexcept;

  /*
    Получение элемента по индексу.
  */
  computing_context &at(size_t);

  /*
    Получение элемента по индексу.
  */
  const computing_context &at(size_t) const;

  /*
    Получение корневого элемента.
  */
  computing_context &root() noexcept;

  /*
    Получение корневого элемента.
  */
  const computing_context &root() const noexcept;

  /*
    Размер дерева, без учета корневого элемента.
  */
  size_t size() const noexcept;

  /*
    Получение индекса последнего элемента.
    Может возвращать индекс корневого элемента.
  */
  size_t index_at_last() const noexcept;

  size_t current_index() const noexcept;

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

private: // Private Hierarhy Interface
  computing_context *get_parent(computing_context *ctx) override;

  std::vector<computing_context *> get_childs(computing_context *ctx) override;

  computing_context *get_root(computing_context *) override;

  void update_context_state(computing_context *) override;

  computing_context *get_context_by_id(size_t id) override;

private: // Data
  struct root_t {
    static constexpr auto id = std::numeric_limits<size_t>::max();
    root_t(FCTree *ctree)
        : style{}, last_child{}, ctx{&root_element_layout, &style, ctree, id} {}
    style style;           // можно унифицировать стиль
    size_t last_child;     // помошник в построении макета
    computing_context ctx; // сам контекст
  } root_{this};
  std::vector<computing_context> nodes;
  // parent\last_brather
  std::stack<size_t> current_{};
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
