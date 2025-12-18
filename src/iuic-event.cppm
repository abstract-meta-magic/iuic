
module;

export module iuic.core:event;
import std;
import :base;
import :kernel;
import :storage.def;
import :state;
export import :key_code;

export namespace iuic {

struct event_extern_components {
  event_extern_components(kernel::root &kernel_) : kernel{kernel_} {}
  struct component {
    component(kernel::root &kernel_) : kernel{kernel_} {}

  protected:
    kernel::root &kernel;
  };

private:
  kernel::root &kernel;

public:
  struct not_function {};

  template <typename T> static consteval auto type(T) -> not_function;

  template <typename T, typename A>
  static consteval auto type(std::function<T(A &)>) -> A;

  struct vis : component {
    void try_visit(iuic::uid_t uid, auto &&call) {
      using type = decltype(type(std::function{call}));
      static_assert(not std::same_as<type, not_function>,
                    "Visit type is void(T&).");

      auto *mem = kernel.memory();
      auto *mtype = kernel::memory_model::type::from<type>();

      if (mem->state(uid, mtype) ==
          kernel::memory_model::object_state::alive_this_type) {
        call(*static_cast<type *>(mem->locate(uid, mtype)));
      }
    };
  } memory{kernel};

  struct : component {
    void attach(state){};
    void detach(state){};
    bool has(state) { return false; };
  } state{kernel};
};

namespace event {
struct event_base {
  event_extern_components utils;
  uid_t object; // can be null
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
  uid_t object;
  kernel::element e;
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
  uid_t object;
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

  event_pack build_pack(const kernel::userspace &kernel) {
    event_pack res;

    for (auto &e : kernel.get_elements()->range()) {
      if (e.meta ^ kernel::element::discarded | kernel::element::arrange) {
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
                  .object = ev.object,
                  .uid = kernel.get_uid(ev.e).value(),
              });
            } else {

              res.local.push_back({
                  .call = ev.call,
                  .policy = kernel.get_event_policy(ev.e).value(),
                  .object = ev.object,
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
    for (auto &&ev : event_pack.global) {
      std::visit(
          [&](auto &&call) {
            using type = std::remove_cvref_t<decltype(call)>;
            if constexpr (std::same_as<global_key_event_fpt, type>) {
              call(event::global::key{
                  event_extern_components{kernel}, ev.object, ev.uid, key, {}});
            }
          },
          ev.call);
    }

    for (auto &&ev : event_pack.local) {
      if (kernel.state()->has(ev.uid, state::base::hovered())) {
        std::visit(
            [&](auto &&call) {
              using type = std::remove_cvref_t<decltype(call)>;
              if constexpr (std::same_as<local_key_event_fpt, type>) {
                call(event::local::key{event_extern_components{kernel},
                                       ev.object,
                                       ev.uid,
                                       key,
                                       {}});
              }
            },
            ev.call);
      }
    }
  };

  // set position without events
  void pointer_set(ui_position position) { pointer_position = position; };

  static bool in__(ui_position position, ui_rect rect) {
    return (position.x >= rect.position.x &&
            position.x <= rect.position.x + rect.size.w &&
            position.y >= rect.position.y &&
            position.y <= rect.position.y + rect.size.h);
  }
  // just move the pointer
  void pointer_move(ui_position position) {
    // TODO
    for (auto &&ev : event_pack.global) {
      std::visit(
          [&](auto &&call) {
            using type = std::remove_cvref_t<decltype(call)>;
            if constexpr (std::same_as<global_pointer_move_event_fpt, type>) {
              call(event::global::mouse{
                  event_extern_components{kernel},
                  ev.object,
                  ev.uid,
                  position,
                  pointer_position,
              });
            }
          },
          ev.call);
    }

    for (auto &&ev : event_pack.local) {
      if (kernel.state()->has(ev.uid, state::base::hovered())) {
        std::visit(
            [&](auto &&call) {
              using type = std::remove_cvref_t<decltype(call)>;
              if constexpr (std::same_as<local_pointer_move_event_fpt, type>) {
                call(event::local::mouse{event_extern_components{kernel},
                                         ev.object,
                                         ev.uid,
                                         position,
                                         pointer_position,
                                         {}});
              }
            },
            ev.call);
      }
    }

    for (auto &h : event_pack.htest) {
      if (in__(position, h.rect)) {
        kernel.state()->attach(h.uid, state::base::hovered());
      } else {
        kernel.state()->detach(h.uid, state::base::hovered());
      }
    }
    pointer_position = position;
  };

  event_reciver(kernel::root &kernel_) : kernel{kernel_} {};

private:
  kernel::root &kernel;
  ui_position pointer_position;
  event_pack event_pack{};
};

void apply_event_pack__(event_reciver &er, event_pack &&pack) {
  // hover scan
  std::swap(er.event_pack, pack);
}
}; // namespace iuic
