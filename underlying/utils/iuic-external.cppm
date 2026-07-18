// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.utils:external;
import std;
import :decl;

// TODO : need full REWORK
// Чего я хочу
// runtime\ct дискрипторы
// для возможности ссылаться на ассеты или ресурсы
export namespace iuic::external {
namespace type {
using decl = utils::ctype<utils::anonim_tag()>;

struct value {
  constexpr auto operator<=>(const value &) const = default;
  const decl *decl{nullptr};

  constexpr value(const type::decl *v) : decl{v} {};
  constexpr value(const type::decl &v) : value{std::addressof(v)} {};
};
}; // namespace type

struct binding {
  virtual ~binding() {};
  virtual type::value type() = 0;
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
