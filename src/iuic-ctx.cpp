
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

void context::self_size() {
  auto &tree = calc_tree.get();

  if (tree.empty()) {
    return;
  }

  auto current = std::rbegin(tree);
  auto end = std::rend(tree);

  for (; current != end; ++current) {
    auto &cc = *current;
    if (cc.element.tags & celement::discarded) {
      continue;
    }
    cc.layout->set_childs_position({&cc, &calc_tree.get_parent(cc)});

    cc.layout->self_size({&cc, &calc_tree.get_parent(cc)});
  }
};
void context::childs_position() {
  // TODO : PARALLEL
  auto &tree = calc_tree.get();

  for (auto &cc : tree) {
    if (cc.element.tags & celement::discarded) {
      continue;
    }
    cc.layout->set_childs_position({&cc, &calc_tree.get_parent(cc)});
  }
}

void context::balancing() {
  // TODO : PARALLEL
  auto &tree = calc_tree.get();
  for (auto &cc : tree) {
    cc.layout->balancing({&cc, &calc_tree.get_parent(cc)});
  }
};

void context::build_render_list() {
  // тут нужно будет отсеять лишние и
  // маркерованные элементы
  // после чего составить простой список
  // отрисовки

  auto &tree = calc_tree.get();

  for (auto &cc : tree) {
    if (cc.element.tags & celement::discarded) {
      continue;
    }
    to_render.push_back(
        {.area{cc.element.full_area},
         .data{frame_render_data{.background{cc.style->background}}}});
  }
}
}; // namespace iuic
