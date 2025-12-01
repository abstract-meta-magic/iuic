
module;

#include <print>
#include <string_view>
#include <variant>
#include <vector>

module iuic.core;
import :layout;

namespace iuic {

void area_request::apply(ui_size sz) { of->apply(sz); };

request_size area_request::value() const {
  auto rq_size = of->get_request();
  if (rq_size) {
    return rq_size.value();
  } else {
    std::println("__area_request__");
    throw computing::violated_order{};
  }
}

request_size measure_child_request::value() const {
  auto rq_size = of->get_request();
  if (rq_size) {
    return rq_size.value();
  } else {
    std::println("__measure_child_request__");
    throw computing::violated_order{};
  }
};

style::cref measure_child_request::style_of() const noexcept {
  return of->get_info().style;
}

std::vector<measure_child_request> frame_measure_utils::get_requests() {
  std::vector<measure_child_request> res;

  for (auto &&cc : ctx->get_hierarchy().interface->get_childs(ctx)) {
    if (cc->is_discarted())
      continue;

    res.push_back({cc});
  };

  return res;
};

void area_request::discard() { of->discard(); };

style::cref area_request::style_of() const { return of->get_info().style; }

style::cref layout_utils_base::self_style() const {
  return ctx->get_info().style;
};

style::cref layout_utils_base::parent_style() const {
  auto parent = ctx->get_hierarchy().interface->get_parent(ctx);
  return parent->get_info().style;
};

style::cref layout_utils_base::root_style() const {
  auto root = ctx->get_hierarchy().interface->get_root(ctx);
  return root->get_info().style;
};

ui_size layout_utils_base::root_size() const {
  return {std::get<upixel_t>(root_style().get_shape().max_size.w),
          std::get<upixel_t>(root_style().get_shape().max_size.h)};
};

upixel_t layout_utils_base::rem(rem_t rem) const noexcept {
  return root_style().get_advance().ephemeral_value * rem;
};

upixel_t layout_utils_base::vh(vh_t vh) const noexcept {
  return std::get<upixel_t>(root_style().get_shape().max_size.h) * vh;
}

upixel_t layout_utils_base::vw(vw_t vw) const noexcept {
  return std::get<upixel_t>(root_style().get_shape().max_size.w) * vw;
}

// TODO : log info
void layout_utils_base::error(std::string_view message) const noexcept {}
void layout_utils_base::log(std::string_view message) const noexcept {}
void layout_utils_base::warning(std::string_view message) const noexcept {}

// AREA
frame_measure_utils::frame_measure_utils(computing::context *ctx_) noexcept
    : layout_utils_base{ctx_} {}

// POSITION

void position_request::apply(ui_position pos) { owner->apply(pos); };

void position_request::discard() { owner->discard(); };

style::cref position_request::style_of() const noexcept {
  return owner->get_info().style;
};

ui_size position_request::size_of() const {
  auto size = owner->get_size();
  if (size) {
    return size.value();
  } else {
    throw computing::violated_order{};
  }
};

std::vector<position_request> frame_position_utils::content() {
  std::vector<position_request> res{};

  for (auto &&cc : ctx->get_hierarchy().interface->get_childs(ctx)) {
    if (cc->is_discarted()) {
      continue;
    }
    res.push_back({cc});
  }

  return res;
};

ui_position frame_position_utils::self_position() const {
  auto rect = ctx->get_rect();
  if (rect) {
    return rect.value().position;
  } else {
    throw computing::violated_order{};
  }
};

ui_size frame_position_utils::self_size() const {
  auto rect = ctx->get_rect();
  if (rect) {
    return rect.value().size;
  } else {
    throw computing::violated_order{};
  }
};

// ARRANGE

std::vector<area_request> frame_arrange_utils::get_requests() {
  std::vector<area_request> res{};

  auto childs = ctx->get_hierarchy().interface->get_childs(ctx);

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

ui_size frame_arrange_utils::self_size() const {
  auto size = ctx->get_size();
  if (size) {
    return size.value();
  } else {
    throw computing::violated_order{};
  };
};

text::token::sequence text_arrange_utils::get_tokens() const { return sq; };
}; // namespace iuic
