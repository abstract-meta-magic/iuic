// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:machine.controller;
import std;
import iuic.underlying;
import :value;

namespace iuic::state::machine {
struct controller {
  virtual bool can_move(state::value) const = 0;

  virtual bool try_move(state::value) = 0;

  virtual bool force_move(state::value) = 0;

  virtual void rethrow() const = 0;

  // visit shared
  constexpr bool try_visit_shared(
      erasure::func_as_decoy<erasure::decoy(erasure::decoy &)> auto &&call) {
    return get_shared_object().try_visit(std::forward<decltype(call)>(call));
  };

  constexpr bool try_visit_shared(
      erasure::func_as_decoy<erasure::decoy(const erasure::decoy &)> auto
          &&call) const {
    return get_shared_object().try_visit(std::forward<decltype(call)>(call));
  };

protected:
  virtual erasure::visited::as_mutable get_shared_object() = 0;
  virtual erasure::visited::as_const get_shared_object() const = 0;
  ~controller() = default;
};

}; // namespace iuic::state::machine
