
module;
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <stack>
#include <string>
#include <vector>

module iuic.core;

namespace iuic {

void context::set_view_size(ui_size size) { view_size = size; };

void context::reset() {
  calc_tree.clear();
  // rl
  // event_registry
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

  std::stack<std::pair<size_t, ui_size>> st;

  size_t begin{tree.size() - 1}, end{0};

  std::vector<ui_size> tmp_childs_list;
  tmp_childs_list.resize(12);

  for (;; --begin) {

    tmp_childs_list.clear();
    for (; not st.empty();) {
      if (st.top().first == begin) {
        tmp_childs_list.push_back(st.top().second);
        st.pop();
        continue;
      }
      break;
    }
    auto &el = tree[begin].elament;
    st.push(
        {tree[begin].parent, el.layout->self_size(*el.style, tmp_childs_list)});
    el.calculated_area.size = st.top().second;

    if (begin == 0) {
      break;
    }
  }

  for (auto &&i : tree) {
    std::cout << "Height : " << i.elament.calculated_area.size.h
              << " | Width : " << i.elament.calculated_area.size.w << " - At : "
              << (i.parent == 0 ? "root" : std::to_string(i.parent))
              << std::endl;

    // ограничитель + набор объектов
    //   i.elament.style.layout(style, std::vector<style>{});
  };
};

void context::balancing() {
  std::cout << "-------------------------" << std::endl
            << "Balancing" << std::endl
            << "-------------------------" << std::endl;
};

void context::build_event_tree() { assert("TODO"); };

void context::build_render_list() { assert("TODO"); };
}; // namespace iuic
