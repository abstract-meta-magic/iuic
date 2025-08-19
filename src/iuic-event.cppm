
module;

#include <any>
#include <cstdint>
#include <functional>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

export module iuic.core:event;
import :base;
import :fct;
import :storage;

export namespace iuic {

struct event_type {
  struct on_enter {
    storage &storage;
    storage_registry_key srk;
  };

  struct on_exit {
    storage &storage;
    storage_registry_key srk;
  };

  struct key {
    storage &storage;
    storage_registry_key srk;
    key_code code;
  };

  struct key_selected : public key {};
};

}; // namespace iuic

namespace iuic {

using void_event_fpt = void (*)();
using on_enter_event_fpt = void (*)(event_type::on_enter);
using on_exit_event_fpt = void (*)(event_type::on_exit);
using key_event_fpt = void (*)(event_type::key);
using key_selected_event_fpt = void (*)(event_type::key_selected);

void void_event_decoy() {};

using variadic_callback =
    std::variant<void_event_fpt, on_enter_event_fpt, on_exit_event_fpt,
                 key_event_fpt, key_selected_event_fpt>;

template <typename T>
concept event_callback_cpt = requires(T &&call) { variadic_callback{call}; };

struct event {
  storage_registry_key data;
  variadic_callback call{&void_event_decoy};
};

struct hit_rect {
  ui_rect rect;
  std::vector<event> events;
};

struct hit_surface {

  // смена поколений событий.
  void change_generation(hit_surface &&);

  void select_element(ui_position);

  bool hit_test(ui_position);

  void pointer_set(ui_position);

  void pointer_move(ui_position);

private:
  std::vector<hit_rect> hits;
  ui_position pointer_position;
  // selected element
  // key buffer
  // capture element or event
};

// главная обязанность - сборка событий
class event_collector {
  //
public:
  using ev = void (*)(storage &, std::any);

  void reset() {};

  void push(event e, size_t id) {
    // TODO
  };

  event_collector() {}

  hit_surface build_surface(const FCTree &ctree) { return {}; };

private:
  // В теории можно сделать 2\3 буффиризацию
  std::vector<event> events;
};

class event_reciver {
  friend void apply_event_hit_surface(hit_surface &&);

public:
  void key(key_code, KeyAction, KeyMod = KeyMod::None){};
  void pointer(ui_position){};

  event_reciver() {};

private:
  // хочется заменить на медод который
  // будет получать уже готовый список
  // событий
};

}; // namespace iuic
