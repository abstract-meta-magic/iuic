
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
import :computing_context;
import :layout.box;

namespace iuic {

/*
  Специайльный layout для корневого элемента.
 */
struct root_element_layout : public layout {
  void self_size(area_utils utils) const noexcept override;

  void set_childs_position(position_utils utils) const noexcept override;

  void balancing(balancing_utils) const noexcept override;
};

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
  computing_context *get_parent(computing_context *ctx) override;

  std::vector<computing_context *> get_childs(computing_context *ctx) override;

  computing_context *get_root(computing_context *) override;

  void update_context_state(computing_context *) override;

  computing_context *get_context_by_id(size_t id) override;

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
