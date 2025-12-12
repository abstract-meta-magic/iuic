
module;

#include <concepts>
#include <map>
#include <print>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

export module iuic.core:event;
import :base;
import :computing.kernel;
import :storage.def;
import :state;
export import :key_code;

export namespace iuic {

struct event_extern_components {
  // text_composers ?
  // style ?
};

namespace event {
struct event_base {
  event_extern_components &utils;
  object_registry_key ork; // can be null
  text_registry_key trk;   // can be null
  uid_t uid;
};
namespace global {
struct key : event_base {
  key_code code;
  ui_position current_mouse_position{};
};

struct mouse : event_base {
  ui_position current_mouse_position{};
  ui_position old_mouse_position{};
};

struct utils {};
}; // namespace global

namespace local {
struct key : event_base {
  key_code code;
  ui_position current_mouse_position{};
  ui_rect rect;
};

struct mouse : event_base {
  ui_position current_mouse_position{};
  ui_position old_mouse_position{};
  ui_rect rect;
};

struct utils {};
}; // namespace local
} // namespace event
}; // namespace iuic
namespace iuic {

// pointer
using global_pointer_move_event_fpt = void (*)(event::global::mouse);
using local_pointer_move_event_fpt = void (*)(event::local::mouse);

// key
using global_key_event_fpt = void (*)(event::global::key);
using local_key_event_fpt = void (*)(event::local::key);

using variadic_callback =
    std::variant<local_key_event_fpt, local_pointer_move_event_fpt,
                 global_key_event_fpt, global_pointer_move_event_fpt>;

template <typename T>
concept event_callback_cpt = requires(T &&call) { variadic_callback{call}; };

template <typename T>
void make_custom(void (*call)(event::global::utils, T)) {
  // тут нужно будет проверить тривиальность копируемости
};

template <typename T> void make_custom(void (*call)(event::local::utils, T)) {};

template <typename T>
concept custom_event_callback_cpt = requires(T &&call) { make_custom(+call); };

// хешировать
struct revent {
  variadic_callback call;
  object_registry_key ork;
  text_registry_key trk;
  computing::element e;
};

struct hovered_test {
  ui_rect rect;
  z_order_t order;
  policy::hovered policy;
  uid_t uid;
};

struct pevent {
  variadic_callback call;
  policy::event policy;
  ork_t ork;
  trk_t trk;
  uid_t uid;
};

struct event_pack {
  std::vector<hovered_test> htest;
  std::vector<pevent> local;
  std::vector<pevent> global;
};

// главная обязанность - сборка событий
class event_collector {
public:
  event_collector() {}

  void reset() {};

  void push(const revent &e) { events.push_back(e); };

  void push(revent &&e) { events.push_back(e); };

  event_pack build_pack(const computing::kernel_user &kernel) {
    event_pack res;

    for (auto &e : kernel.get_elements()->range()) {
      if (e.meta ^ computing::element::discarded |
          computing::element::arrange) {
        if (auto rect = kernel.get_rect_bordered(e)) {

          res.htest.push_back(
              hovered_test{.rect = rect.value(),
                           .order = kernel.get_zorder(e).value(),
                           .policy = kernel.get_hovered_policy(e).value(),
                           .uid = kernel.get_uid(e).value()});
        }
      }
    }

    // sort htest by order

    for (auto &ev : events) {
      // TODO : Make pack

      if (auto policy = kernel.get_hovered_policy(ev.e);
          policy.value() == policy::hovered::none) {
        continue;
      }

      // sort global\local

      std::visit(
          [&](auto &call) {
            using type = std::remove_cvref_t<decltype(call)>;
            if constexpr (std::same_as<type, global_key_event_fpt> ||
                          std::same_as<type, global_pointer_move_event_fpt>) {
              res.global.push_back({
                  .call = ev.call,
                  .policy = kernel.get_event_policy(ev.e).value(),
                  .ork = ev.ork,
                  .trk = ev.trk,
                  .uid = kernel.get_uid(ev.e).value(),
              });
            } else {

              res.local.push_back({
                  .call = ev.call,
                  .policy = kernel.get_event_policy(ev.e).value(),
                  .ork = ev.ork,
                  .trk = ev.trk,
                  .uid = kernel.get_uid(ev.e).value(),
              });
            }
          },
          ev.call);
    };

    events.clear();

    return res;
  };

private:
  // В теории можно сделать 2\3 буффиризацию
  // промежуточное решение
  // возможно прийдеться отдельно регестрировать и
  // обробатывать on_enter on_exit
  std::vector<revent> events;
};

// ОСНОВНАЯ ОБЯЗАННОСТЬ :
// корректная подготовка и отправка событий
class event_reciver final {
  friend void apply_event_pack__(event_reciver &, event_pack &&);

public:
  void key(key_code key) {
    // TODO
  };

  // set position without events
  void pointer_set(ui_position position) {};

  static bool in__(ui_position position, ui_rect rect) {
    return (position.x >= rect.position.x &&
            position.x <= rect.position.x + rect.size.w &&
            position.y >= rect.position.y &&
            position.y <= rect.position.y + rect.size.h);
  }

  // just move the pointer
  void pointer_move(ui_position position) {
    // TODO

  };

  event_reciver(computing::kernel_root &kernel_) : kernel{kernel_} {};

private:
  computing::kernel_root &kernel;
  ui_position pointer_position;
  event_pack event_pack{};
};

void apply_event_pack__(event_reciver &er, event_pack &&pack) {
  std::swap(er.event_pack, pack);
}
}; // namespace iuic
