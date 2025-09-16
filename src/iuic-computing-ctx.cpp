
module;

#include <expected>
#include <print> // remove
#include <variant>
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

std::variant<const frame_layout *, const text_layout *>
computing_context::get_layout() const noexcept {
  if (element.attribute_tags & celement::attribute_tags_t::text) {
    return {text_layout};
  }
  return {frame_layout};
};

// TODO : Rename
std::expected<ui_rect, celement::stage_t>
computing_context::get_rect() const noexcept {
  if (element.stage == celement::stage_t::complite) {
    return element.full_area;
  }
  return std::unexpected{element.stage};
};

std::expected<ui_size, celement::stage_t>
computing_context::get_size() const noexcept {
  if (element.stage == celement::stage_t::position) {
    return element.applyed_size;
  }
  return std::unexpected{element.stage};
};

std::expected<request_size, celement::stage_t>
computing_context::get_request() const noexcept {
  if (element.stage == celement::stage_t::arrange) {
    return element.area_request;
  }
  return std::unexpected{element.stage};
};

bool computing_context::is_discarted() const noexcept {
  return element.attribute_tags & celement::attribute_tags_t::discarded;
};

void computing_context::apply(request_size rq) {
  if (element.stage == celement::stage_t::measure) {
    element.area_request = rq;
    element.stage = celement::stage_t::arrange;
  } else {
    throw violated_computing_order{};
  }
}

void computing_context::apply(ui_size sz) {
  if (element.stage == celement::stage_t::arrange) {
    element.applyed_size = sz;
    element.stage = celement::stage_t::position;
  } else {
    throw violated_computing_order{};
  }
}

void computing_context::apply(ui_position pos) {
  if (element.stage == celement::stage_t::position) {
    auto sz = element.applyed_size;
    element.full_area = ui_rect{pos, sz};
    element.stage = celement::stage_t::complite;
  } else {
    throw violated_computing_order{};
  }
}

void computing_context::discard() {
  element.attribute_tags += celement::attribute_tags_t::discarded;
  hierarhy->update_context_state(this);
};
}; // namespace iuic
