// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:decl;
import std;

export namespace iuic::state {
struct decl final {

  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  // force static\global
  const decl *self{this};

private:
  consteval decl() noexcept {};
  decl(const decl &) = default;

public:
  template <const decl &of> static consteval decl instance_of() {
    static constexpr decl _{};
    return _;
  };
};

} // namespace iuic::state
