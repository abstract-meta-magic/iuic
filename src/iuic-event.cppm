
module;

#include <map>
#include <variant>
#include <vector>

export module iuic.core:event;
import :base;
import :fct;
import :storage;
export import :key_code;

export namespace iuic {

// in version 0.2
struct element_state_selector {

  bool is_hovered(uid_t uid) { return false; };

  bool is_active(uid_t uid) { return false; };

  bool is_focused(uid_t uid) { return false; };

  void set_active(uid_t uid) {};

  void set_focused(uid_t uid) {};

  void unset_focused(uid_t uid) {};

  void unset_active(uid_t uid) {};

  void unset_focused() {};

  void unset_active() {};
};

struct event_type {
  struct event_base {
    storage &storage;
    element_state_selector &selector;
    storage_registry_key srk;
  };

  struct on_enter : public event_base {};

  struct on_exit : public event_base {};

  struct key : public event_base {
    key_code code;
  };

  struct key_u : public key {
    uid_t uid;
  };

  struct key_f : public key_u {};

  struct key_a : public key_u {};
};
}; // namespace iuic
namespace iuic {

using void_event_fpt = void (*)();
using on_enter_event_fpt = void (*)(event_type::on_enter);
using on_exit_event_fpt = void (*)(event_type::on_exit);
using key_event_fpt = void (*)(event_type::key);
using key_unique_event_fpt = void (*)(event_type::key_u);
using key_focused_event_fpt = void (*)(event_type::key_f);
using key_active_event_fpt = void (*)(event_type::key_a);

void void_event_decoy() {};

using variadic_callback =
    std::variant<void_event_fpt, on_enter_event_fpt, on_exit_event_fpt,
                 key_event_fpt, key_focused_event_fpt, key_unique_event_fpt,
                 key_active_event_fpt>;

template <typename T>
concept event_callback_cpt = requires(T &&call) { variadic_callback{call}; };

// хешировать
struct event {
  storage_registry_key data;
  variadic_callback call{&void_event_decoy};
};

// Нужно проработать интерфейст
// для удобства внутреннего использования
// Нужно лучше продумать роль этого класса
// в системе событий
struct hit_surface {
  using hit_map_t = std::map<ui_rect, std::vector<event>>;

  hit_surface() {};
  explicit hit_surface(hit_map_t &&hit_map_) : hit_map{std::move(hit_map_)} {};
  explicit hit_surface(const hit_map_t &hit_map_) : hit_map{hit_map_} {};

  // смена поколений событий.
  void change_generation(hit_surface &&other) {
    // TODO : update
    std::swap(other.hit_map, hit_map);
  };

  void select_element(ui_position);

  bool hit_test(ui_position);

  void pointer_set(ui_position);

  void pointer_move(ui_position);

  void key_pressed(key_code code) {
    static storage st;
    static element_state_selector sl;
    for (auto &region : hit_map) {
      for (auto &e : region.second) {
        std::visit(
            [&e, code](auto &&call) {
              if constexpr (std::is_same_v<std::remove_cvref_t<decltype(call)>,
                                           key_event_fpt>) {
                call(event_type::key{st, sl, e.data, code});
              }
            },
            e.call);
      };
    }
  };

private:
  hit_map_t hit_map;
  ui_position pointer_position;
  // selected element
  // key buffer
  // capture element or event
};

// главная обязанность - сборка событий
class event_collector {
public:
  event_collector() {}

  void reset() {};

  void push(const event &e, size_t id) { events[id].push_back(e); };

  void push(event &&e, size_t id) { events[id].push_back(std::move(e)); };

  hit_surface build_surface(const FCTree &ctree) {
    hit_surface::hit_map_t res;

    for (auto &&e : events) {
      auto &rect = ctree.at(e.first).get_rect();
      res[rect] = std::move(e.second);
    }

    return hit_surface{std::move(res)};
  };

private:
  // В теории можно сделать 2\3 буффиризацию
  // промежуточное решение
  std::map<size_t, std::vector<event>> events;
};

// позже будет разработан uuid
// он будет использоватся для сохранения
// состояния между кадрами
// ОСНОВНАЯ ОБЯЗАННОСТЬ :
// корректная подготовка и отправка событий
class event_reciver {
  friend void apply_event_hit_surface(event_reciver &, hit_surface &&);

public:
  void key(key_code key) { surface.key_pressed(key); };

  void pointer(ui_position){};

  // in version 0.2
  void key_buff(key_code);

  // in version 0.2
  void key_buff_dispatch(key_code);

  event_reciver() {};

private:
  size_t active_id;
  size_t action_id;
  // хочется заменить на медод который
  // будет получать уже готовый список
  // событий
  hit_surface surface{};
};

void apply_event_hit_surface(event_reciver &er, hit_surface &&surface) {
  er.surface.change_generation(std::move(surface));
}

}; // namespace iuic
