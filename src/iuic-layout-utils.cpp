
module;
#include <string_view>
#include <vector>
module iuic.core;
import :base;
import :ftc;

namespace iuic {
void area_request::apply() {
  of->element.full_area.size = requiest_value;
  of->element.tags += celement::request_strong_applyed;
  to->applyed_group.push_back(of);
};

void area_request::apply(ui_size sz) {
  of->element.full_area.size = sz;
  to->applyed_group.push_front(of);
};

const ui_size &area_request::value() const noexcept { return requiest_value; }

void area_request::discard() {
  // TODO : Не происходит пометка всех дочерних по иерархии элементов
  // как discarted
  // что позволяет некоторым объектам отоброжаться вызвая баги отрисовки
  // нужно разабраться с системой discarded
  // подсказка 1 2 2 2 2 1 2 3 4 5 4 3 2 2 1
  //               obj 1 - ^         ^ - obj 2
  // если (1) помечен как discarted то можно помечать все объекты влоть до (2)
  // его ближаешого родствиника как discarted, что делаеться на уровне FTC
  of->applyed_group.clear(); // нет дочерних объектов нет проблемм )
  of->element.tags += celement::discarded;
};

const style &area_request::style_of() const { return *of->style; }

const style &layout_utils_base::self_style() const { return *self->style; };
const style &layout_utils_base::parent_style() const { return *parent->style; };
const style &layout_utils_base::viewport_style() const {
  // TODO : Сделать viewport style ref
  static style _;

  return _;
};

// TODO : log info
void layout_utils_base::error(std::string_view message) const noexcept {}
void layout_utils_base::log(std::string_view message) const noexcept {}
void layout_utils_base::warning(std::string_view message) const noexcept {}

// AREA

bool area_utils::has_request() const noexcept {
  return not self->requests.empty();
};
area_request area_utils::next_request() {
  if (has_request()) {
    auto res = self->requests.top();
    self->requests.pop();
    return res;
  }
  // error
  throw "EMPTY";
};

void area_utils::request_size(ui_size sz) {
  if (self->element.tags &
      (celement::discarded + celement::size_request_dispatched)) {
    // TODO : В Debug сборке тут можно сделать исключение
    return;
  }
  parent->requests.push({self, parent, sz});
  self->element.tags += celement::size_request_dispatched;

  discard_remaining_requiests();
};
void area_utils::viewport_request_size(ui_size sz) {
  // WARNING : Пака вообще не работает
  if (self->element.tags &
      (celement::discarded + celement::size_request_dispatched)) {
    // TODO : В Debug сборке тут можно сделать исключение
    return;
  }
  parent->requests.push({self, parent, sz});
  self->element.tags += celement::size_request_dispatched + celement::absolute;

  discard_remaining_requiests();
};

void area_utils::self_discard() {
  if (self->element.tags &
      (celement::discarded + celement::size_request_dispatched)) {
    // TODO : В Debug сборке тут можно сделать исключение
    return;
  }
  self->element.tags += celement::discarded;

  discard_remaining_requiests();
};

void area_utils::discard_remaining_requiests() {
  auto requests = self->requests;
  for (; not requests.empty();) {
    auto &rq = requests.top();

    rq.discard();

    requests.pop();
  }
};

// POSITION

void position_request::apply(ui_position pos) {
  owner->element.full_area.position = pos;
  owner->element.tags += celement::position_inspected;
};

void position_request::discard() {
  owner->element.tags += celement::discarded;
};

const style &position_request::style_of() const noexcept {
  return *owner->style;
};

const ui_size &position_request::size_of() const noexcept {
  return owner->element.full_area.size;
};

std::vector<position_request> position_utils::content() {
  std::vector<position_request> res{};

  for (auto &&cc : self->applyed_group) {
    if (cc->element.tags & celement::discarded) {
      continue;
    }
    res.push_back({cc});
  }

  return res;
};

const ui_position &position_utils::self_position() const noexcept {
  return self->element.full_area.position;
};

// BALANCING

bool balancing_utils::is_requiest_applied() const noexcept {
  return self->element.full_area.size != ui_size{0, 0};
};

bool balancing_utils::is_strong_applied() const noexcept {
  return self->element.tags & celement::request_strong_applyed;
};
}; // namespace iuic
