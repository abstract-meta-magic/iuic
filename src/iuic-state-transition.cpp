
module;
#include <vector>

module iuic.core;
import :state.transition;

namespace iuic {

void state_transition_scheduler::apply(std::size_t index,
                                       state_transition tr) noexcept {
  cencel(index);
  transitions.insert({index, tr});
};

void state_transition_scheduler::cencel(std::size_t index) noexcept {
  if (transitions.contains(index)) {
    transitions.at(index).destroy();
    transitions.extract(index);
  }
};

std::pair<pseudo_state, pseudo_state>
state_transition_scheduler::state_of(std::size_t index) const noexcept {
  if (transitions.contains(index)) {
    auto &state = transitions.at(index);
    return {state.from, state.to};
  } else {
    return {pseudo_state::null(), pseudo_state::null()};
  };
};

void state_transition_scheduler::process() noexcept {
  std::vector<std::size_t> to_delete{};
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

}; // namespace iuic
