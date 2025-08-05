
module;

#include <any>
#include <cstdint>
#include <functional>
#include <string_view>
#include <type_traits>
#include <vector>

export module iuic.core:event;
import :base;
import :storage;

namespace iuic {

struct key_event_transfer_data {
  storage_registry_key srk;
  storage &storage;
};

// Может разделить на painter_event && key_event
struct key_event {
  using event_call_t = void (*)(key_event_transfer_data);
  using trigger_call_t = bool (*)();
  key_event_transfer_data data;
  size_t element_id;
  event_call_t call;
  trigger_call_t trigger;
  enum property : std::uint8_t {
    // ...
  };
};

// событие
// привязанный элемент
// данные для события

class tmp_event_registry {
  //
public:
  using ev = void (*)(storage &, std::any);

  template <auto call>
  void registry_key_event(size_t id, key_code kc, KeyMod km, KeyAction ka) {
    // TODO : Body
    // TODO : REFACTOR
    key_event ke{{{}, storage}};

    ke.call = call;
  };

  template <auto call>
  void registry_key_event(size_t id, key_code kc, KeyMod km, KeyAction ka,
                          storage_registry_key srk) {
    // TODO : Body
    // TODO : REFACTOR
    key_event ke{{{}, storage}};

    ke.call = call;

    events.push_back(std::move(ke));
  };

  tmp_event_registry(storage &storage_) : storage{storage_} {}

private:
  storage &storage;
  std::vector<key_event> events;

  void (*captured_element)(void *);
};

struct hehe {
  template <typename T> void operator()(T) {};
};

class event_reciver {
public:
  void key(key_code, KeyAction, KeyMod = KeyMod::None){};
  void pointer(ui_position){};

  event_reciver(tmp_event_registry &r) : registry{r} {};

private:
  tmp_event_registry &registry;
};

}; // namespace iuic
