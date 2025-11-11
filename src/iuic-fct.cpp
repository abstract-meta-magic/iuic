

module;

#include <iostream>
#include <limits>
#include <print>
#include <vector>

module iuic.core;
import :computing.tree;

namespace iuic::computing {

tree::range_based_for_proxy tree::range_for() { return {*this}; };

tree::const_range_based_for_proxy tree::range_for() const { return {*this}; };

tree::reverse_range_based_for_proxy tree::reverse_range_for() {
  return {*this};
};

tree::const_reverse_range_based_for_proxy tree::reverse_range_for() const {
  return {*this};
};

tree::range_based_for_proxy::range_based_for_proxy(tree &ctree)
    : begin_{&ctree.nodes[0], (void *)(&ctree.nodes.back() + 1)} {}

tree::const_range_based_for_proxy::const_range_based_for_proxy(
    const tree &ctree)
    : begin_{&ctree.nodes[0], (const void *)(&ctree.nodes.back() + 1)} {}

tree::reverse_range_based_for_proxy::reverse_range_based_for_proxy(tree &ctree)
    : begin_{&ctree.nodes.back(), (void *)(&ctree.nodes[0] - 1)} {}

tree::const_reverse_range_based_for_proxy::const_reverse_range_based_for_proxy(
    const tree &ctree)
    : begin_{&ctree.nodes.back(), (void *)(&ctree.nodes[0] - 1)} {}

// tree

void tree::reset() {
  nodes.clear();
  parent = {}; // ...
  parent.push({root_.id, root_.id});
  root_.ctx.element.stage = element::stage_t::measure;
};

void tree::add(style::ref style, const frame_layout *layout) {
  // Проблемма с дочерними объектами root element
  // Может на stack сразу ложить root element ?
  auto parent_id = not parent.empty() ? parent.top().first : root_.id;

  nodes.push_back({layout, style, this, parent_id});

  auto &node = nodes.back();
  auto id = nodes.size() - 1;

  if (not parent.empty()) {

    auto &famaly = parent.top();

    if (famaly.first != famaly.second) {
      nodes[famaly.second].get_hierarchy().brother = id;
    }

    famaly.second = id;
  } else {
    if (root_.last_child != root_.id) {
      nodes[root_.last_child].get_hierarchy().brother = id;
    }

    root_.last_child = id;
  }

  parent.push({id, id});
  current_.push(id);
};

// TODO : fix this
// it's text add, not frame == other algo
void tree::add(style::ref style, const text_layout *layout) {
  // Проблемма с дочерними объектами root element
  // Может на stack сразу ложить root element ?
  auto parent_id = not parent.empty() ? parent.top().first : root_.id;

  nodes.push_back({layout, style, this, parent_id});

  auto &node = nodes.back();
  auto id = nodes.size() - 1;

  if (not parent.empty()) {

    auto &famaly = parent.top();

    if (famaly.first != famaly.second) {
      nodes[famaly.second].get_hierarchy().brother = id;
    }

    famaly.second = id;
  } else {
    if (root_.last_child != root_.id) {
      nodes[root_.last_child].get_hierarchy().brother = id;
    }

    root_.last_child = id;
  }

  parent.push({id, id});
  current_.push(id);
};

void tree::up() {
  parent.pop();
  current_.pop();
};

context *tree::last() noexcept {
  // REFACTOR THIS
  if (nodes.empty()) {
    return &root_.ctx;
  }

  return &nodes.back();
};

const context *tree::last() const noexcept { return last(); }

context *tree::current() noexcept {
  if (current_.empty()) {
    return &root_.ctx;
  }
  return &nodes[current_.top()];
}

const context *tree::current() const noexcept {
  if (current_.empty()) {
    return &root_.ctx;
  }
  return &nodes[current_.top()];
}

context *tree::root() noexcept { return &root_.ctx; }

const context *tree::root() const noexcept { return &root_.ctx; }

context *tree::at(size_t id) { return &nodes[id]; }

const context *tree::at(size_t id) const { return &nodes[id]; }

size_t tree::size() const noexcept { return nodes.size(); };

size_t tree::index_at_last() const noexcept {
  if (nodes.empty()) {
    return root_.id;
  }

  return size() - 1;
};

size_t tree::current_index() const noexcept {
  if (current_.empty()) {
    return root_.id;
  }
  return current_.top();
};

void tree::set_root_size(ui_size sz) {
  root_.style.shape.max_size = {sz.w, sz.h};
};

void tree::print_tree() const noexcept {
  // BORKEN
};

context *tree::get_parent(context *ctx) {
  if (ctx == &root_.ctx) {
    return ctx;
  } else if (ctx->get_hierarchy().parent ==
             std::numeric_limits<size_t>::max()) {
    return &root_.ctx;
  }
  return &nodes[ctx->get_hierarchy().parent];
};

std::vector<context *> tree::get_childs(context *ctx) {
  // WARNING : Очень хрупко
  // Требуеться рефакторинг
  std::vector<context *> res{};

  if (nodes.empty()) {
    return res;
  }

  auto hierarchy = ctx->get_hierarchy().interface;

  auto current = (ctx == &root_.ctx) ? &nodes[0] : (ctx + 1);

  if (current == &nodes.back() + 1 || get_parent(current) != ctx) {
    return res;
  }

  for (;;) {
    if (not current->is_discarted()) {
      res.push_back(current);
    }

    if (auto &h = current->get_hierarchy(); h.parent == h.brother) {
      break;
    }

    current = &nodes[current->get_hierarchy().brother];
  }

  return res;
}

context *tree::get_root(context *) { return &root_.ctx; };

void tree::update_context_state(context *ctx) {
  if (ctx->is_discarted()) {
    ctx->element.attribute_tags += element::attribute_tags_t::discarded;

    if (auto id = get_id(ctx); id <= nodes.size()) {
      if (nodes[id + 1].hierarchy.parent == id) {
        auto begin = &nodes[id + 1];
        auto end = begin;

        for (; end->hierarchy.brother != end->hierarchy.parent;) {
          end = &nodes[end->hierarchy.brother];
        }

        // TOTO : std::foreach
        for (; begin < end; ++begin) {
          begin->element.attribute_tags += element::attribute_tags_t::discarded;
        }
      }
    }
  }
};

context *tree::get_context_by_id(size_t id) { return &nodes[id]; };
size_t tree::get_id(context *ctx) {
  // WARNING : unsafe
  return std::distance(&nodes[0], ctx);
}
}; // namespace iuic::computing
