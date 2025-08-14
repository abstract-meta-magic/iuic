
module;
#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <iterator>
#include <print>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

module iuic.core;

namespace iuic {

void context::set_view_size(ui_size size) { ctree.set_root_size(size); };

const std::vector<relement> &context::get_tree() { return to_render; }

void context::reset() {
  ctree.reset();
  to_render.clear();
};

void context::self_size() {

  // warning

  auto rtree = ctree.reverse_range_for();

  for (auto &&cc : ctree.reverse_range_for()) {
    if (cc.is_discarted()) {
      continue;
    }
    cc.get_layout().self_size({&cc});
  }

  // отвратительно
  ctree.get_root().get_layout().self_size({&ctree.get_root()});
};
void context::childs_position() {
  // TODO : PARALLEL
  ctree.get_root().get_layout().set_childs_position({&ctree.get_root()});

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }
    cc.get_layout().set_childs_position({&cc});
  }
}

void context::balancing() {
  // TODO : PARALLEL

  for (auto &cc : ctree.range_for()) {
    cc.get_layout().balancing({&cc});
  }
};

void context::build_render_list() {
  // тут нужно будет отсеять лишние и
  // маркерованные элементы
  // после чего составить простой список
  // отрисовки

  for (auto &cc : ctree.range_for()) {
    if (cc.is_discarted()) {
      continue;
    }
    to_render.push_back(
        {.area{cc.get_rect()},
         .data{frame_render_data{.background{cc.get_style().background}}}});
  }
}
}; // namespace iuic
