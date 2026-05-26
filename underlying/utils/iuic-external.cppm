// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.utils:external;
import std;
import :decl;

export namespace iuic::external {

using type = utils::ctype<utils::anonim_tag()>;

constexpr inline type extern_null{};

struct binding {

  constexpr virtual ~binding() = default;

  constexpr virtual const type &type() const noexcept { return extern_null; };

  constexpr virtual std::string_view info() const noexcept {
    return "Extern binding interface";
  };
};

namespace capabilities {

struct decl {
  constexpr decl() = default;
  decl(const decl &) = delete;
  decl &operator=(const decl &) = delete;
  decl(decl &&) = delete;
  decl &operator=(decl &&) = delete;

  constexpr bool operator==(const decl &other) const {
    return this == std::addressof(other);
  }
};

struct ref {
  constexpr ref(const decl &d) : value{std::addressof(d)} {};
  constexpr ref(const decl *d) : value{d} {};

  constexpr bool operator==(const ref &other) const {
    return value == other.value;
  }
  const decl *value{};
};

}; // namespace capabilities

}; // namespace iuic::external
