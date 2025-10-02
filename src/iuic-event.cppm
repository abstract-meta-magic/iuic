
module;

#include <map>
#include <print>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

export module iuic.core:event;
import :base;
import :fct;
import :storage.def;
import :state;
export import :key_code;

export namespace iuic {

struct event_extern_components {
  state_holder &state;
  object_storage &object;
  text_storage &text;
  // text_composers ?
  // style ?
};

namespace event {
struct event_base {
  event_extern_components &utils;
  object_registry_key ork; // can be null
  text_registry_key trk;   // can be null
};
namespace global {
struct key : event_base {
  uid_t uid;
  key_code code;
};

struct pointer_move {};
}; // namespace global

namespace local {
struct key : event_base {
  uid_t uid;
  key_code code;
};

struct pointer_move {};
}; // namespace local
} // namespace event
}; // namespace iuic
namespace iuic {

// pointer
using global_pointer_move_event_fpt = void (*)(event::local::pointer_move);
using local_pointer_move_event_fpt = void (*)(event::local::pointer_move);

// key
using global_key_event_fpt = void (*)(event::global::key);
using local_key_event_fpt = void (*)(event::local::key);

using variadic_callback =
    std::variant<local_key_event_fpt, local_pointer_move_event_fpt,
                 global_key_event_fpt, global_pointer_move_event_fpt>;

template <typename T>
concept event_callback_cpt = requires(T &&call) { variadic_callback{call}; };

// хешировать
struct revent {
  variadic_callback call;
  object_registry_key ork;
  text_registry_key trk;
  size_t id;
  uid_t uid;
};

// Нужно проработать интерфейст
// для удобства внутреннего использования
// Нужно лучше продумать роль этого класса
// в системе событий
struct event_pack {
  using hit_map_t = std::map<ui_rect, std::vector<revent>>;
  using key_event_map = std::map<uid_t, std::vector<revent>>;

  key_event_map key;
  hit_map_t pointer;
};

// главная обязанность - сборка событий
class event_collector {
public:
  event_collector() {}

  void reset() {};

  void push(const revent &e) { events.push_back(e); };

  void push(revent &&e) { events.push_back(e); };

  event_pack build_pack(const FCTree &ctree) {
    // Bag тут не пропускаются discarded элементы
    event_pack res;

    for (auto &e : events) {
      // TODO : Make pack
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
  friend void apply_event_hit_surface(event_reciver &, event_pack &&);

public:
  void key(key_code key) {
    for (auto &&e : event_pack.key[0]) {
      std::visit(
          [&, this](auto &call) {
            if constexpr (std::is_invocable_v<
                              std::remove_cvref_t<decltype(call)>,
                              iuic::event::global::key>) {
              call(iuic::event::global::key{
                  extern_component,
                  e.ork,
                  e.trk,
                  e.uid,
                  key,
              });
            }
          },
          e.call);
    }

    // global

    // local
  };

  // set position without events
  void pointer_set(ui_position position) {};

  // just move the pointer
  void pointer_move(ui_position position) {
    // TODO : body
    pointer_position = position;
  };

  // in version 0.2
  void key_buff(key_code);

  // in version 0.2
  void key_buff_dispatch(key_code);

  event_reciver(object_storage &ostorage_, text_storage &tstorage_,
                state_holder &state_)
      : extern_component{state_, ostorage_, tstorage_} {};

private:
  event_extern_components extern_component;
  ui_position pointer_position;

  event_pack event_pack{};
};

void apply_event_hit_surface(event_reciver &er, event_pack &&ep) {
  std::swap(er.event_pack, ep);
}

}; // namespace iuic
