
module;

#include <expected>
#include <print> // remove
#include <variant>
#include <vector>
module iuic.core;
import :computing_context;

namespace iuic {

std::variant<const frame_layout *, const text_layout *>
computing_context::get_layout() const noexcept {
  if (element.attribute_tags & celement::attribute_tags_t::text) {
    return {element.text_layout};
  }
  return {element.frame_layout};
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
  if (element.stage == celement::stage_t::frame_position) {
    return element.applyed_size;
  } else if (element.stage == celement::stage_t::text_present) {
    return element.applyed_size;
  } else if (element.stage == celement::stage_t::text_position) {
    return element.applyed_text_present->rect.size;
  }
  return std::unexpected{element.stage};
};

std::expected<request_size, celement::stage_t>
computing_context::get_request() const noexcept {
  if (element.stage == celement::stage_t::frame_arrange ||
      element.stage == celement::stage_t::text_arrange) {
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
    if (element.attribute_tags & celement::attribute_tags_t::text) {
      element.stage = celement::stage_t::text_arrange;
    } else {
      element.stage = celement::stage_t::frame_arrange;
    }
  } else {
    throw violated_computing_order{};
  }
}

void computing_context::apply(ui_size sz) {
  if (element.stage == celement::stage_t::frame_arrange) {
    element.applyed_size = sz;
    element.stage = celement::stage_t::frame_position;
  } else if (element.stage == celement::stage_t::text_arrange) {
    element.applyed_size = sz;
    element.stage = celement::stage_t::text_present;
  } else {
    throw violated_computing_order{};
  }
}

void computing_context::apply(text::present *tp) {
  if (element.stage == celement::stage_t::text_present) {
    element.applyed_text_present = tp;
    element.stage = celement::stage_t::text_position;
  } else {
    throw violated_computing_order{};
  };
};

void computing_context::apply(ui_position pos) {
  if (element.stage == celement::stage_t::frame_position) {
    auto sz = element.applyed_size;
    element.full_area = ui_rect{pos, sz};
    element.stage = celement::stage_t::complite;
  } else if (element.stage == celement::stage_t::text_position) {
    element.applyed_text_present->rect.position = pos;
    element.stage = celement::stage_t::complite;
  } else {
    throw violated_computing_order{};
  }
}

void computing_context::discard() {
  element.attribute_tags += celement::attribute_tags_t::discarded;
  hierarchy.interface->update_context_state(this);
};

computing_context::info_t &computing_context::get_info() { return info; };

const computing_context::info_t &computing_context::get_info() const {
  return info;
};

computing_context::hierarchy_t &computing_context::get_hierarchy() {
  return hierarchy;
};

const computing_context::hierarchy_t &computing_context::get_hierarchy() const {
  return hierarchy;
};
}; // namespace iuic
