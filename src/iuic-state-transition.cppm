
module;

#include <coroutine>
#include <map>
#include <utility>
#include <variant>
#include <vector>

export module iuic.core:state.transition;
import :state;

namespace iuic {
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
struct state_transition_scheduler {

  state_transition_scheduler(state_holder &state_) : state{state_} {};

  void apply(std::size_t index, state_transition tr) noexcept;

  void cencel(std::size_t index) noexcept;

  std::pair<pseudo_state, pseudo_state>
  state_of(std::size_t index) const noexcept;

  void process() noexcept;

private:
  state_holder &state;
  std::map<size_t, state_transition> transitions;
};
}; // namespace iuic
