
module;
#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <iterator>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

module iuic.core;

namespace iuic {

void context::set_view_size(ui_size size) { view_size = size; };

const std::vector<relement> &context::get_tree() { return to_render; }

void context::reset() {
  calc_tree.clear();
  to_render.clear();
};

// У меня есть собранные элементы
// нужно сделать первичный просчет и выдать
// базовую деагностику
struct ui_limiter {
  ui_size min, max;
};

void context::primary_calculations() {
  std::cout << "-------------------------" << std::endl
            << "Collecting" << std::endl
            << "-------------------------" << std::endl;
  auto &tree = calc_tree.get();
  // down to up

  if (tree.empty()) {
    return;
  }

  using it_t = typename std::remove_cvref_t<decltype(tree)>::iterator;

  std::stack<std::pair<size_t, size_t>> st;

  size_t begin{tree.size() - 1}, end{0};

  celement a;
  // TOTO : Refactor it's penis

  static std::vector<celement> ep_el{};
  static std::deque<size_t> ep_ch{};

  for (;; --begin) {
    auto &node = calc_tree.get(begin);

    if (not node.childs.empty()) {
      ep_ch = node.childs;
      tree[begin].calculated_area.size =
          node.layout->self_size(*tree[begin].style, {&tree, &ep_ch});
    } else {
      tree[begin].calculated_area.size =
          node.layout->self_size(*tree[begin].style, {&ep_el, &ep_ch});
    }

    if (begin == 0) {
      break;
    }
  }

  end = tree.size() - 1;
  for (; begin != end; ++begin) {
    auto &node = calc_tree.get(begin);
    if (not node.childs.empty()) {
      ep_ch = node.childs;
      node.layout->set_childs_position(tree[begin], {&tree, &ep_ch});
    } else {
      ep_ch.clear();
      node.layout->set_childs_position(tree[begin], {&tree, &ep_ch});
    }
  }

  begin = 0;
  for (; begin != end; ++begin) {
    auto &el = tree[begin];
    std::cout << "Height : " << el.calculated_area.size.h
              << " | Width : " << el.calculated_area.size.w << std::endl;

    // ограничитель + набор объектов
    //   i.elament.style.layout(style, std::vector<style>{});
  };

  for (auto &el : tree) {
    to_render.push_back({.area{el.calculated_area},
                         .data{frame_render_data{el.style->background}}});
  }
};

void context::balancing() {
  std::cout << "-------------------------" << std::endl
            << "Balancing" << std::endl
            << "-------------------------" << std::endl;
};

void context::build_event_tree() { assert("TODO"); };

void context::build_render_list() { assert("TODO"); };
}; // namespace iuic
