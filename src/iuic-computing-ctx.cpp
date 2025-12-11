
module;

#include <expected>
#include <print> // remove
#include <variant>
#include <vector>

module iuic.core;
import :computing.kernel;
import :text.present;

namespace iuic::computing {

std::variant<const frame_layout *, const text_layout *>
context::get_layout() const noexcept {
  if (element.attribute_tags & element::attribute_tags_t::text) {
    return {element.text_layout};
  }
  return {element.frame_layout};
};

// TODO : Rename
std::expected<ui_rect, element::stage_t> context::get_rect() const noexcept {
  if (element.stage == element::stage_t::complite) {
    return element.full_area;
  }
  return std::unexpected{element.stage};
};

std::expected<ui_size, element::stage_t> context::get_size() const noexcept {
  if (element.stage == element::stage_t::frame_position) {
    return element.applyed_size;
  } else if (element.stage == element::stage_t::text_position) {
    return element.applyed_size;
  }
  return std::unexpected{element.stage};
};

std::expected<request_size, element::stage_t>
context::get_request() const noexcept {
  if (element.stage == element::stage_t::frame_arrange ||
      element.stage == element::stage_t::text_arrange) {
    return element.area_request;
  }
  return std::unexpected{element.stage};
};

bool context::is_discarted() const noexcept {
  return element.attribute_tags & element::attribute_tags_t::discarded;
};

void context::apply(request_size rq) {
  if (element.stage == element::stage_t::measure) {
    element.area_request = rq;
    if (element.attribute_tags & element::attribute_tags_t::text) {
      element.stage = element::stage_t::text_arrange;
    } else {
      element.stage = element::stage_t::frame_arrange;
    }
  } else {
    throw violated_order{};
  }
}

void context::apply(ui_size sz) {
  if (element.stage == element::stage_t::frame_arrange) {
    element.applyed_size = sz;
    element.stage = element::stage_t::frame_position;
  } else if (element.stage == element::stage_t::text_arrange) {
    element.applyed_size = sz;
    element.stage = element::stage_t::text_position;
  } else {
    throw violated_order{};
  }
}

void context::apply(ui_position pos) {
  if (element.stage == element::stage_t::frame_position) {
    auto sz = element.applyed_size;
    element.full_area = ui_rect{pos, sz};
    element.stage = element::stage_t::complite;
  } else if (element.stage == element::stage_t::text_position) {
    auto sz = element.applyed_size;
    element.full_area = ui_rect{pos, sz};
    element.stage = element::stage_t::complite;
  } else {
    throw violated_order{};
  }
}

void context::discard() {
  element.attribute_tags += element::attribute_tags_t::discarded;
  hierarchy.interface->update_context_state(this);
};

context::info_t &context::get_info() { return info; };

const context::info_t &context::get_info() const { return info; };

context::hierarchy_t &context::get_hierarchy() { return hierarchy; };

const context::hierarchy_t &context::get_hierarchy() const {
  return hierarchy;
};
}; // namespace iuic::computing
