module;

#include <cstdint>

export module iuic.core:policy;

export namespace iuic::policy {

struct shared {
  unsigned up{0};
};

struct unique {};

struct indexed {};

enum class hovered : std::uint8_t {
  none,
  propagate, // true -> go
  block,     // true -> stop
};

enum class event : std::uint8_t { propagate, block };
}; // namespace iuic::policy
