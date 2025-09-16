
module;

#include <iostream>
#include <stacktrace>

#include <print>
#include <string_view>
#include <vector>
module iuic.core;
import :layout;

namespace iuic {

void area_request::apply(ui_size sz) { of->apply(sz); };

request_size area_request::value() const noexcept {
  auto rq_size = of->get_request();
  if (rq_size) {
    return rq_size.value();
  } else {
    throw violated_computing_order{};
  }
}

std::vector<measure_child_request> frame_measure_utils::get_requests() {
  std::vector<measure_child_request> res;

  for (auto &&cc : ctx->get_childs()) {
    if (cc->is_discarted())
      continue;

    res.push_back({cc});
  };

  return res;
};

void area_request::discard() { of->discard(); };

const style &area_request::style_of() const { return of->get_style(); }

const style &layout_utils_base::self_style() const { return ctx->get_style(); };

const style &layout_utils_base::parent_style() const {
  return ctx->get_parent()->get_style();
};

const style &layout_utils_base::root_style() const {
  // TODO : Сделать viewport style ref
  static style _;

  return _;
};

pixel_t layout_utils_base::rem_width(rem_t rem) const {
  return std::get<ui_size>(root_style().ephemeral_value).w * rem;
};

pixel_t layout_utils_base::rem_hieght(rem_t rem) const {
  return std::get<ui_size>(root_style().ephemeral_value).h * rem;
};

pixel_t layout_utils_base::vh(vh_t vh) const {
  return std::get<upixel_t>(root_style().shape.max_size.h) * vh;
}

pixel_t layout_utils_base::vw(vw_t vw) const {
  return std::get<upixel_t>(root_style().shape.max_size.h) * vw;
}

// TODO : log info
void layout_utils_base::error(std::string_view message) const noexcept {}
void layout_utils_base::log(std::string_view message) const noexcept {}
void layout_utils_base::warning(std::string_view message) const noexcept {}

// AREA
frame_measure_utils::frame_measure_utils(computing_context *ctx_) noexcept
    : layout_utils_base{ctx_} {}

// POSITION

void position_request::apply(ui_position pos) { owner->apply(pos); };

void position_request::discard() { owner->discard(); };

const style &position_request::style_of() const noexcept {
  return owner->get_style();
};

ui_size position_request::size_of() const noexcept {
  auto size = owner->get_size();
  if (size) {
    return size.value();
  } else {
    throw violated_computing_order{};
  }
};

std::vector<position_request> frame_position_utils::content() {
  std::vector<position_request> res{};

  for (auto &&cc : ctx->get_childs()) {
    if (cc->is_discarted()) {
      continue;
    }
    res.push_back({cc});
  }

  return res;
};

ui_position frame_position_utils::self_position() const noexcept {
  auto position = ctx->get_rect();
  if (position) {
    return position.value().position;
  } else {
    throw violated_computing_order{};
  }
};

// ARRANGE

std::vector<area_request> frame_arrange_utils::get_requests() {
  std::vector<area_request> res{};

  auto childs = ctx->get_childs();

  for (auto &&ch : childs) {
    // TODO : Refactor
    auto rq = ch->get_request();

    if (not rq) {
      continue;
    }

    res.push_back({ch});
  }
  return res;
}
ui_size frame_arrange_utils::get_size() const noexcept {
  auto size = ctx->get_size();
  if (size) {
    return size.value();
  } else {
    throw violated_computing_order{};
  };
};
}; // namespace iuic
