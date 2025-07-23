
module;

#include <stack>
#include <vector>

export module iuic.core:ftc;
import :base;

namespace iuic {

// flat tree of calculations
struct FTC {
  struct node {
    celement elament;
    // 0 - is root
    std::size_t parent;
    std::size_t childs_end;
  };

  void clear() {
    nodes.clear();
    parent = {}; // ...
  };

  void add(const style &style, render_data &&data) {
    static constexpr box_layout box{};
    auto parent_id = parent.empty() ? 0 : parent.top();
    nodes.push_back(node{{&style}, parent_id});
    nodes.back().elament.layout = &box; // WARNING tmp
    auto &e = nodes.back().elament;
    if (parent_id != 0) {
      nodes[parent_id].childs_end = nodes.size();
    }
    parent.push(nodes.size() - 1);
  };

  void up() { parent.pop(); };

  // TODO: Refactor
  std::vector<node> &get();

  node &get_current();

private:
  std::vector<node> nodes;
  // current type
  std::stack<std::size_t> parent;
};
}; // namespace iuic
