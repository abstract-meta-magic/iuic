// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:type;
import std;
import iuic.underlying.erasure;

export namespace iuic::external::type {
struct decl {
  consteval decl() noexcept : self{this}, base{nullptr} {}
  consteval decl(const decl *base_) noexcept : self{this}, base{base_} {}

  const decl *self;
  const decl *base;
};

struct value {
  constexpr auto operator<=>(const value &) const = default;
  const decl *decl{nullptr};

  constexpr value(const type::decl *v) : decl{v} {};
  constexpr value(const type::decl &v) : value{std::addressof(v)} {};
};

}; // namespace iuic::external::type
