

module;

#include <iostream>
#include <limits>
#include <print>
#include <vector>

module iuic.core;
import :ftc;

namespace iuic {

void computing_context::set_brother(size_t id) { brother = id; };

void computing_context::unset_brother() { brother = parent; };

size_t computing_context::get_parent_id() const { return parent; };

size_t computing_context::get_brother_id() const { return brother; };

computing_context *computing_context::get_parent() {
  if (not hierarhy) {
    std::cout << "null hi" << std::endl;
    std::cout << "null hi" << std::endl;
    return nullptr;
  }
  return hierarhy->get_parent(this);
};

std::vector<computing_context *> computing_context::get_childs() {
  return hierarhy->get_childs(this);
};

const style &computing_context::get_style() const noexcept { return *style; };

const layout &computing_context::get_layout() const noexcept {
  return *layout;
};

// TODO : Rename
const ui_rect &computing_context::get_rect() const noexcept {
  return element.full_area;
};

bool computing_context::is_discarted() const noexcept {
  return element.state_tags & celement::state_tags_t::discarded;
};

bool computing_context::is_area_request_dispatched() const noexcept {
  return element.area_tags & celement::area_tags_t::area_request_dispatched;
};

void computing_context::set_area(ui_size size, celement::area_tags_t tag) {

  if (tag & celement::area_tags_t::area_request_dispatched) {
    if (element.area_tags & celement::area_tags_t::area_request_dispatched) {
      // TODO : error ?
    }
    element.full_area.size = size;
    element.area_tags += tag;
  } else if (tag & celement::area_tags_t::area_request_deferred) {
    element.area_tags += tag;
  } else if (tag & celement::area_tags_t::area_request_strong_appyed) {
    // нужно бы проверять был ли запрос
    element.area_tags += tag;
    element.area_tags -= celement::area_tags_t::area_request_dispatched;
  } else if (tag & celement::area_tags_t::area_request_soft_appyed) {
    // нужно бы проверять был ли запрос
    element.full_area.size = size;
    element.area_tags += tag;
    element.area_tags -= celement::area_tags_t::area_request_dispatched;
  }
};

void computing_context::set_position(ui_position pos,
                                     celement::position_tags_t tag) {
  if (tag & celement::position_tags_t::set_position_is_applyed) {
    if (element.position_tags & tag) {
      // error ?
    }

    element.full_area.position = pos;

    element.position_tags += tag;
  } else if (tag & celement::position_tags_t::set_position_is_deferred) {
    element.position_tags += tag;
  }
};

void computing_context::discard() {
  element.state_tags += celement::state_tags_t::discarded;
  hierarhy->update_context_state(this);
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
  parent.push({root.id, root.id});
};

void FCTree::add(const style &style, const layout *layout) {
  // Проблемма с дочерними объектами root element
  // Может на stack сразу ложить root element ?
  auto parent_id = not parent.empty() ? parent.top().first : root.id;

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
    if (root.last_child != root.id) {
      nodes[root.last_child].set_brother(id);
    }

    root.last_child = id;
  }

  parent.push({id, id});
};

void FCTree::up() { parent.pop(); };

computing_context &FCTree::last() {
  // REFACTOR THIS
  if (nodes.empty()) {
    return root.ctx;
  }

  return nodes.back();
};

computing_context &FCTree::get_root() { return root.ctx; }

void FCTree::set_root_size(ui_size sz) { root.style.shape.max_size = sz; };

void FCTree::print_tree() const noexcept {

  size_t id{0};

  std::println("------------------------------------------");
  for (auto &&node : nodes) {
    auto parent = node.get_parent_id() == root.id
                      ? "root"
                      : std::to_string(node.get_parent_id());
    auto brother = (node.get_brother_id() == node.get_parent_id() ||
                    node.get_brother_id() == root.id)
                       ? "last"
                       : std::to_string(node.get_brother_id());
    auto &rect = node.get_rect();

    std::println("Id : {},Parent : {},Brother : {},Position : x:{},y:{},Size "
                 ": w:{},h:{}  |",
                 id, parent, brother, rect.position.x, rect.position.y,
                 rect.size.w, rect.size.h);
    ++id;
  }
  std::println("------------------------------------------");
};
}; // namespace iuic
