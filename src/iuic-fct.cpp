

module;

#include <iostream>
#include <limits>
#include <print>
#include <vector>

module iuic.core;
import :fct;

namespace iuic {

void root_element_layout::self_size(area_utils utils) const noexcept {
  auto &style = utils.self_style();
  utils.set_hard_size(style.shape.max_size);

  auto requests = utils.get_requests();

  for (auto &&rq : requests) {
    rq.apply();
  }
};

void root_element_layout::set_childs_position(
    position_utils utils) const noexcept {
  auto position = ui_position{0, 0};

  auto def = position;

  auto content = utils.content();

  for (auto &&rq : content) {
    auto &style = rq.style_of();

    position.y += style.positioning.margin.top;
    position.x += style.positioning.margin.left;
    rq.apply(position);
    auto &size = rq.size_of();
    position.y += size.h;
    position.x = def.x;
  }
};

void root_element_layout::balancing(balancing_utils) const noexcept {
    // ake box layout
};

FCTree::range_based_for_proxy FCTree::range_for() { return {*this}; };

FCTree::const_range_based_for_proxy FCTree::range_for() const {
  return {*this};
};

FCTree::reverse_range_based_for_proxy FCTree::reverse_range_for() {
  return {*this};
};

FCTree::const_reverse_range_based_for_proxy FCTree::reverse_range_for() const {
  return {*this};
};

FCTree::range_based_for_proxy::range_based_for_proxy(FCTree &ctree)
    : begin_{&ctree.nodes[0], (void *)(&ctree.nodes.back() + 1)} {}

FCTree::const_range_based_for_proxy::const_range_based_for_proxy(
    const FCTree &ctree)
    : begin_{&ctree.nodes[0], (const void *)(&ctree.nodes.back() + 1)} {}

FCTree::reverse_range_based_for_proxy::reverse_range_based_for_proxy(
    FCTree &ctree)
    : begin_{&ctree.nodes.back(), (void *)(&ctree.nodes[0] - 1)} {}

FCTree::const_reverse_range_based_for_proxy::
    const_reverse_range_based_for_proxy(const FCTree &ctree)
    : begin_{&ctree.nodes.back(), (void *)(&ctree.nodes[0] - 1)} {}

// FCTree

void FCTree::reset() {
  nodes.clear();
  parent = {}; // ...
  parent.push({root_.id, root_.id});
};

void FCTree::add(const style &style, const layout *layout) {
  // Проблемма с дочерними объектами root element
  // Может на stack сразу ложить root element ?
  auto parent_id = not parent.empty() ? parent.top().first : root_.id;

  nodes.push_back({layout, &style, this, parent_id});

  auto &node = nodes.back();
  auto id = nodes.size() - 1;

  if (not parent.empty()) {

    auto &famaly = parent.top();

    if (famaly.first != famaly.second) {
      nodes[famaly.second].set_brother(id);
    }

    famaly.second = id;
  } else {
    if (root_.last_child != root_.id) {
      nodes[root_.last_child].set_brother(id);
    }

    root_.last_child = id;
  }

  parent.push({id, id});
  current.push(id);
};

void FCTree::up() {
  parent.pop();
  current.pop();
};

computing_context &FCTree::last() noexcept {
  // REFACTOR THIS
  if (nodes.empty()) {
    return root_.ctx;
  }

  return nodes.back();
};

computing_context &FCTree::last() const noexcept { return last(); }

computing_context &FCTree::root() noexcept { return root_.ctx; }

const computing_context &FCTree::root() const noexcept { return root_.ctx; }

computing_context &FCTree::at(size_t id) { return nodes[id]; }

const computing_context &FCTree::at(size_t id) const { return nodes[id]; }

size_t FCTree::size() const noexcept { return nodes.size(); };

size_t FCTree::index_at_last() const noexcept {
  if (nodes.empty()) {
    return root_.id;
  }

  return size() - 1;
};

size_t FCTree::current_index() const noexcept {
  if (current.empty()) {
    return root_.id;
  }
  return current.top();
};

void FCTree::set_root_size(ui_size sz) { root_.style.shape.max_size = sz; };

void FCTree::print_tree() const noexcept {

  size_t id{0};

  std::println("------------------------------------------");
  for (auto &&node : nodes) {
    auto parent = node.get_parent_id() == root_.id
                      ? "root"
                      : std::to_string(node.get_parent_id());
    auto brother = (node.get_brother_id() == node.get_parent_id() ||
                    node.get_brother_id() == root_.id)
                       ? "last"
                       : std::to_string(node.get_brother_id());
    auto &rect = node.get_rect();

    auto ds = node.element.state_tags & celement::state_tags_t::discarded
                  ? "discarted"
                  : "ok";

    auto mem = celement::state_tags_t::discarded;

    mem += celement::state_tags_t::discarded;

    //   if (mem & celement::state_tags_t::discarded) {
    // std::println("hahahahahhh");
    // }

    std::println("Id : {},Parent : {},Brother : {},Position : x:{},y:{},Size "
                 ": w:{},h:{} |",
                 id, parent, brother, rect.position.x, rect.position.y,
                 rect.size.w, rect.size.h);
    std::println("State : {}", ds);
    ++id;
  }
  std::println("------------------------------------------");
};

computing_context *FCTree::get_parent(computing_context *ctx) {
  if (ctx->get_parent_id() == std::numeric_limits<size_t>::max()) {
    return &root_.ctx;
  }
  return &nodes[ctx->get_parent_id()];
};

std::vector<computing_context *> FCTree::get_childs(computing_context *ctx) {
  // WARNING : Очень хрупко
  // Требуеться рефакторинг
  std::vector<computing_context *> res{};

  if (nodes.empty()) {
    return res;
  }

  auto current = ctx == &root_.ctx ? &nodes[0] : ctx + 1;

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
}

computing_context *FCTree::get_root(computing_context *) { return &root_.ctx; };

void FCTree::update_context_state(computing_context *ctx) {

  // TODO : Refactor this
  if (ctx->element.state_tags & celement::state_tags_t::discarded) {
    computing_context *end{ctx};

    if (ctx->parent == ctx->brother && not(ctx->parent == root_.id)) {
      computing_context *parent{&nodes[ctx->parent]};

      for (;;) {
        if (parent->brother == parent->parent) {
          if (parent->parent == root_.id) {
            end = &nodes.back() + 1;
            break;
          } else {
            parent = &nodes[parent->parent];
          }
        } else {
          end = &nodes[parent->brother];
        }
      }
    } else if (ctx->parent == ctx->brother) {
      end = &nodes.back() + 1;
    } else {
      end = &nodes[ctx->brother];
    };

    auto current = ctx;

    for (; current < end; ++current) {
      current->element.state_tags += celement::state_tags_t::discarded;
    }
  }
};

computing_context *FCTree::get_context_by_id(size_t id) { return &nodes[id]; };
}; // namespace iuic
