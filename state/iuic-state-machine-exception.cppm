// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:machine.exception;
import std;

namespace iuic::state::machine {
struct impossible_transition : std::runtime_error {
  impossible_transition()
      : std::runtime_error{"Inpossible transition from Machine-Stay"} {};
};

struct weak_machine : std::runtime_error {
  weak_machine() : std::runtime_error{"Weak machine handler"} {};
};
}; // namespace iuic::state::machine
