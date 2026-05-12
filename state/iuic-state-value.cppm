// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:value;
import std;
import :decl;

export namespace iuic::state {
struct value final {

  constexpr value() : decl{nullptr} {};

  constexpr value(const decl *state_ptr) : decl{state_ptr} {};
  constexpr value(const decl &state_ref) : decl{&state_ref} {};

  constexpr bool operator==(const value &other) const noexcept {
    return other.decl == decl;
  };

  operator std::size_t() const noexcept { return (std::size_t)decl; };

  constexpr bool operator!=(const value &other) const noexcept {
    return not(other.decl == decl);
  };

  constexpr value &operator=(const value &other) noexcept {
    decl = other.decl;
    return *this;
  };

  const decl *decl{nullptr};
};

} // namespace iuic::state

namespace std {
export template <> struct std::hash<iuic::state::value> {
  std::size_t operator()(const iuic::state::value &s) const noexcept {
    return s;
  }
};

}; // namespace std
