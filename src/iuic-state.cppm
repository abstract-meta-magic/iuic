
module;

#include <chrono>
#include <coroutine>
#include <map>
#include <print>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

export module iuic.core:state;
import :base;

export namespace iuic {

struct state final {
  struct decl;
  struct base;

  state() = delete;

  constexpr state(const decl *state_ptr) : value{state_ptr} {};
  constexpr state(const decl &state_ref) : value{&state_ref} {};

  constexpr bool operator==(const state &other) const noexcept {
    return (size_t)other.value == (size_t)value;
  };

  constexpr bool operator!=(const state &other) const noexcept {
    return not(other.value == value);
  };

  constexpr state &operator=(const state &other) noexcept {
    value = other.value;
    return *this;
  };

  static constexpr state null() { return nullptr; };

  const decl *value{nullptr};
};

struct state::decl final {
  consteval decl() noexcept {};

  decl(const decl &) = delete;
  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  constexpr operator const decl *() const noexcept { return this; }

  // force static\global
  const decl *self{this};
};
struct state::base {
  static constexpr state hovered() {
    static constexpr state::decl _;
    return _;
  };
};
}; // namespace iuic
