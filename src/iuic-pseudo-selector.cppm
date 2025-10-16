
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

struct hovered_r__ {
  bool hovered(uid_t uid) const { return hovered__.contains(uid); };

protected:
  std::unordered_set<uid_t> hovered__;
};

// read \ write pseudo
// read hovered

struct pseudo_base__ {
  friend struct pseudo_proxy__;

  struct data {
    std::chrono::time_point<std::chrono::steady_clock> last_change;
    pseudo_state old;
    pseudo_state from;
    pseudo_state to;
  };

protected:
  std::unordered_map<uid_t, data> pseudo__;
  std::unordered_map<uid_t, data> pseudo_defer__; // sync ?
};

struct pseudo_proxy__ {
  pseudo_proxy__(uid_t uid_, pseudo_base__ &owner_)
      : uid{uid_}, owner{owner_} {};

  operator pseudo_state() const && {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid).to;
    } else {
      return pseudo_state::null();
    };
  };

  pseudo_proxy__ &operator=(pseudo_state state_) && {

    if (owner.pseudo__.contains(uid)) {
      auto &state = owner.pseudo__.at(uid);

      state.last_change = std::chrono::steady_clock::now();
      state.old = state.from;
      state.from = state.to;
      state.to = state_;
    } else {
      owner.pseudo__.insert(
          {uid, pseudo_base__::data{{std::chrono::steady_clock::now()},
                                    {pseudo_state::null()},
                                    {pseudo_state::null()},
                                    state_}});
    };
    return *this;
  };
  bool operator==(pseudo_state other) const && noexcept {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid).to == other;
    } else {
      return false;
    };
  };

  pseudo_state from() const noexcept {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid).from;
    } else {
      return pseudo_state::null();
    };
  };

  pseudo_state to() const noexcept {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid).to;
    } else {
      return pseudo_state::null();
    };
  }

  auto time_point() const noexcept {
    if (owner.pseudo__.contains(uid)) {
      return owner.pseudo__.at(uid).last_change;
    } else {
      return std::chrono::time_point<std::chrono::steady_clock>{};
    }
  };

private:
  uid_t uid;
  pseudo_base__ &owner;
};

struct pseudo_r__ : virtual pseudo_base__ {
  pseudo_state pseudo(uid_t uid) const noexcept {
    if (pseudo__.contains(uid)) {
      return pseudo__.at(uid).to;
    } else {
      return pseudo_state::null();
    }
  };

  void pseudo_default(uid_t uid, pseudo_state state) {
    if (not pseudo__.contains(uid)) {
      pseudo__.insert(
          {uid, pseudo_base__::data{{std::chrono::steady_clock::now()},
                                    {pseudo_state::null()},
                                    {pseudo_state::null()},
                                    state}});
    }
  };
};

struct pseudo_rw__ : virtual pseudo_base__ {
  pseudo_proxy__ pseudo(uid_t uid) noexcept { return {uid, *this}; };
};

struct state_holder : virtual pseudo_rw__, virtual hovered_r__ {};

struct const_state_holder : virtual pseudo_r__, virtual hovered_r__ {};

struct managed_state_holder : public state_holder, public const_state_holder {
  void update(std::unordered_set<uid_t> &&hovered) {
    std::swap(hovered__, hovered);
  };
};

export {
  struct null_t {
  } constexpr inline null{};

  struct ok_t {
  } constexpr inline ok{};

  struct err_t {
  } constexpr inline err{};
}
struct state_tansition_promise_type;

using state_corutine_handle_type =
    std::coroutine_handle<state_tansition_promise_type>;

export struct state_transition : public state_corutine_handle_type {
  state_transition(state_corutine_handle_type h)
      : state_corutine_handle_type{h} {};
  using promise_type = state_tansition_promise_type;

  pseudo_state from{pseudo_state::null()};
  pseudo_state to{pseudo_state::null()};
  uid_t uid;
};

struct state_tansition_promise_type {
  state_transition get_return_object() {
    return state_corutine_handle_type::from_promise(*this);
  };

  std::suspend_always initial_suspend() noexcept { return {}; }

  std::suspend_always final_suspend() noexcept { return {}; }

  std::suspend_always yield_value(std::variant<ok_t, null_t>) { return {}; };

  void return_value(iuic::pseudo_state state) { ret = state; };

  void unhandled_exception() {};

  pseudo_state ret{pseudo_state::null()};
};

// raname
struct pseudo_state_transition_handler {

  pseudo_state_transition_handler(state_holder &state_) : state{state_} {};

  // не забыть destroy
  void apply(size_t index, state_transition &&tr) {
    cencel(index);
    transitions.insert({index, tr});
  };

  void cencel(size_t index) {
    if (transitions.contains(index)) {
      transitions.at(index).destroy();
      transitions.extract(index);
    }
  };

  std::pair<pseudo_state, pseudo_state>
  transition_state(size_t index) const noexcept {
    if (transitions.contains(index)) {
      auto &state = transitions.at(index);
      return {state.from, state.to};
    } else {
      return {pseudo_state::null(), pseudo_state::null()};
    };
  };

  void process() {
    std::vector<size_t> to_delete{};
    for (auto &&[_, tr] : transitions) {
      if (not tr || tr.done()) {
        to_delete.push_back(_);
        continue;
      }

      tr.resume();

      if (tr.promise().ret != pseudo_state::null()) {
        state.pseudo(tr.uid) = tr.promise().ret;
        to_delete.push_back(_);
      }
    }

    for (auto &&i : to_delete) {
      transitions.erase(i);
    }
  };

private:
  state_holder &state;
  std::map<size_t, state_transition> transitions;
};
}; // namespace iuic
