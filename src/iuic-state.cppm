
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

namespace iuic {
struct pseudo_state_decl final {
  consteval pseudo_state_decl() noexcept {};

  pseudo_state_decl(const pseudo_state_decl &) = delete;
  pseudo_state_decl &operator=(const pseudo_state_decl &) = delete;
  pseudo_state_decl(pseudo_state_decl &&) = delete;
  pseudo_state_decl &operator=(pseudo_state_decl &&) = delete;

  constexpr operator const pseudo_state_decl *() const noexcept { return this; }

  // force static\global
  const pseudo_state_decl *self{this};
};

export struct pseudo_state final {
  template <typename T> static pseudo_state make() {
    return make__<std::remove_cvref_t<T>>();
  };

  pseudo_state() = delete;

  constexpr pseudo_state(const pseudo_state_decl *state_ptr)
      : state{state_ptr} {};
  constexpr pseudo_state(const pseudo_state_decl &state_ref)
      : state{state_ref} {};

  constexpr bool operator==(const pseudo_state &other) const noexcept {
    return (size_t)other.state == (size_t)state;
  };

  constexpr bool operator!=(const pseudo_state &other) const noexcept {
    return not(other.state == state);
  };

  constexpr pseudo_state &operator=(const pseudo_state &other) noexcept {
    state = other.state;
    return *this;
  };

  static constexpr pseudo_state null() { return nullptr; };

  // private:
  template <typename> static constexpr const pseudo_state_decl *make__() {
    static constexpr pseudo_state_decl _{};
    return _;
  };
  const pseudo_state_decl *state{nullptr};
};

// read \ write pseudo
// read hovered

struct transition_info {
  std::chrono::time_point<std::chrono::steady_clock> last_change{
      time_t::clock::now()};
  pseudo_state old{pseudo_state::null()};
  pseudo_state from{pseudo_state::null()};
  pseudo_state to{pseudo_state::null()};
};

struct state_holder_base {
  friend struct pseudo_proxy__;

protected:
  std::unordered_set<uid_t> hovered__;
  std::unordered_map<uid_t, transition_info> pseudo__;
  std::unordered_map<uid_t, transition_info> pseudo_defer__; // sync ?
};

struct pseudo_proxy__ {
  pseudo_proxy__(uid_t uid_, state_holder_base &owner_)
      : uid{uid_}, owner{owner_} {};

  operator pseudo_state() const noexcept;

  pseudo_proxy__ &operator=(pseudo_state state_) noexcept;

  bool operator==(pseudo_state other) const noexcept;

  pseudo_state from() const noexcept;

  pseudo_state to() const noexcept;

  time_t time_point() const noexcept;

private:
  uid_t uid;
  state_holder_base &owner;
};

export struct const_state_holder : protected state_holder_base {
  bool hovered(uid_t uid) const noexcept;

  pseudo_state pseudo(uid_t uid) const noexcept;

  void pseudo_init_value(uid_t uid, pseudo_state) noexcept;
};

export struct state_holder : const_state_holder {
  pseudo_proxy__ pseudo(uid_t uid) noexcept;
};

struct managed_state_holder : public state_holder {
  void update(std::unordered_set<uid_t> &&hovered) {
    std::swap(hovered__, hovered);
  };
};

}; // namespace iuic
