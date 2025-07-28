
module;

#include <any>
#include <cstdint>
#include <functional>
#include <string_view>
#include <vector>

export module iuic.core:event;
import :base;
import :storage;

namespace iuic {

struct event {
  using event_call_t = void (*)(storage_ref &);
  ui_rect rect;
  event_call_t call;
  storage_ref ref;
  enum property : std::uint8_t {
    // ...
  };
};

void test() {
  auto a = [](storage &storage) {

  };
};

// событие
// привязанный элемент
// данные для события

class tmp_event_registry {
  //
  using ev = void (*)(storage &, std::any);

  template <typename Call>
    requires requires(storage &storage) { Call(storage); }
  void reg(Call call, ui_rect rect) {};

  std::vector<event> events;
};

class event_reciver {
public:
  void key(std::string_view, KeyAction, KeyMod = KeyMod::None){};
  void pointer(ui_position){};

  event_reciver(tmp_event_registry &r) : registry{r} {};

private:
  tmp_event_registry &registry;
};

}; // namespace iuic
