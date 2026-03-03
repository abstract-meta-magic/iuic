

export module iuic.core:event.collector;
import std;
import :event;

namespace iuic::event {
struct collector {
  void push(value e);

  std::span<value> get(units::uid);

private:
  std::vector<value> events;
};
}; // namespace iuic::event
