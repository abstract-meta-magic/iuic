
module;
#include <print>
#include <string_view>
#include <vector>
module iuic.core;
import :base;
import :ftc;

namespace iuic {
void area_request::apply() {
  // тут вроде как можно проверить на is_area_request_dispatched
  of->set_area({}, celement::area_tags_t::area_request_strong_appyed);
};

void area_request::apply(ui_size sz) {
  of->set_area(sz, celement::area_tags_t::area_request_soft_appyed);
};

const ui_size &area_request::value() const noexcept {
  return of->get_rect().size;
}

std::vector<area_request> area_utils::get_requests() {
  std::vector<area_request> res;

  for (auto &&cc : ctx->get_childs()) {
    if (cc->is_discarted() || not cc->is_area_request_dispatched())
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

// TODO : log info
void layout_utils_base::error(std::string_view message) const noexcept {}
void layout_utils_base::log(std::string_view message) const noexcept {}
void layout_utils_base::warning(std::string_view message) const noexcept {}

// AREA
area_utils::area_utils(computing_context *ctx_) noexcept
    : layout_utils_base{ctx_} {}

void area_utils::request_size(ui_size sz) {
  if (ctx->is_discarted()) {
    // TODO : В Debug сборке тут можно сделать исключение
    return;
  }

  ctx->set_area(sz, celement::area_tags_t::area_request_dispatched);
};

void area_utils::viewport_request_size(ui_size sz) {
  // WARNING : Пака вообще не работает
  if (ctx->is_discarted()) {
    // TODO : В Debug сборке тут можно сделать исключение
    return;
  }

  ctx->set_area(sz, celement::area_tags_t::area_request_dispatched);
};

void area_utils::set_hard_size(ui_size sz) {
  ctx->set_area(sz, celement::area_tags_t::area_request_strong_appyed);
}

void area_utils::self_discard() {
  if (ctx->is_discarted()) {
    // TODO : В Debug сборке тут можно сделать исключение
    return;
  }

  ctx->discard();
};

void area_utils::discard_remaining_requiests() {
  // WRONG
};

// POSITION

void position_request::apply(ui_position pos) {
  owner->set_position(pos, celement::position_tags_t::set_position_is_applyed);
};

void position_request::discard() { owner->discard(); };

const style &position_request::style_of() const noexcept {
  return owner->get_style();
};

const ui_size &position_request::size_of() const noexcept {
  return owner->get_rect().size;
};

std::vector<position_request> position_utils::content() {
  std::vector<position_request> res{};

  for (auto &&cc : ctx->get_childs()) {
    if (cc->is_discarted()) {
      std::println("Position discard");
      continue;
    }
    std::println("Position rq");
    res.push_back({cc});
  }

  return res;
};

const ui_position &position_utils::self_position() const noexcept {
  return ctx->get_rect().position;
};

// BALANCING

bool balancing_utils::is_requiest_applied() const noexcept {
  // WRONG
  return false;
};

bool balancing_utils::is_strong_applied() const noexcept {
  // WRONG
  return false;
};
}; // namespace iuic
