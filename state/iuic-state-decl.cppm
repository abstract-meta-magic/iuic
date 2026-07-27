// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:decl;
import std;

export namespace iuic::state {
struct decl final {

  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;
  decl(const decl &) = delete;

  consteval decl() noexcept {};

  // force static\global
  const decl *self{this};
};

} // namespace iuic::state
