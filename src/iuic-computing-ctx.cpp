
module;
#include <vector>
module iuic.core;
import :computing_context;

namespace iuic {

void computing_context::set_brother(size_t id) { brother = id; };

void computing_context::unset_brother() { brother = parent; };

size_t computing_context::get_parent_id() const { return parent; };

size_t computing_context::get_brother_id() const { return brother; };

computing_context *computing_context::get_parent() {
  if (not hierarhy) {
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
}; // namespace iuic
