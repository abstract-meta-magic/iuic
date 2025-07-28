
module;

#include <deque>
#include <iostream>
#include <stack>
#include <vector>

export module iuic.core:ftc;
import :base;
import :layout.box;

namespace iuic {

// flat tree of calculations
struct FTC {
  struct node {
    // 0 - is root
    const layout *layout{nullptr};
    std::deque<size_t> childs;
  };

  void clear() {
    nodes.clear();
    elements.clear();
    parent = {}; // ...
  };

  void add(const style &style, const layout *layout) {
    static constexpr box_layout box{};
    elements.push_back({elements.size(), &style});
    nodes.push_back(node{layout});

    if (not parent.empty()) {
      std::cout << "parent:" << parent.top() << std::endl;
      std::cout << "parent set child:" << elements.size() - 1 << std::endl;
      nodes[parent.top()].childs.push_back(elements.size() - 1);
    }
    parent.push(elements.size() - 1);
  };

  void up() { parent.pop(); };

  // TODO: Refactor
  std::vector<celement> &get();
  node &get(size_t);

  node &get_current();

private:
  std::vector<node> nodes;
  std::vector<celement> elements;
  // current type
  std::stack<std::size_t> parent;
};
}; // namespace iuic
