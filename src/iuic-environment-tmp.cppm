
export module iuic.core:environment.tmp;
import iuic.underlying;
import iuic.state;
import :machine.dispatcher;
import :key_code;
import :style;
import :event.collector;

namespace iuic::environment {
struct tmp {
  void text();
  style::sheet style;
  event::collector event;
  void polices();
};
}; // namespace iuic::environment

constexpr auto lol = []() consteval { return 1; }();
