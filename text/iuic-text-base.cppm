// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:base;
import iuic.underlying;
import std;

namespace iuic::text {

export struct semantic {
  consteval semantic() : value{this} {};
  const semantic *value;

  constexpr auto operator<=>(const semantic &) const = default;
};

// Glyph ID
export enum class gid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max(),
  // | Требует доработки
  // V
  unexpected = null - 1,
  empty = unexpected - 1,
};
// Glyph context ID
export enum class gcid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max(),
  // | Требует доработки
  // V
  unexpected = null - 1,
  special = unexpected - 1, // used with semantic , like carriage
  empty = special - 1,
};
// Semantic group ID
export enum class sgid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};
// Logical sequence group ID
export enum struct lsgid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};
// Folding ID
export enum class eid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};

// Cell metrics ID
// TODO: rename to glyph metrics ID
export enum class cmid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};
// Layout metrics ID
// TODO: rename to context metrics ID
export enum class lmid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};
// Glyph info ID
export enum class giid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};
// Meta info ID
export enum class miid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};
// Compose ID
export enum class cid : std::uint64_t {
  null = std::numeric_limits<std::uint64_t>::max()
};

// Unicode codepoint
export enum class codepoint : char32_t {};

}; // namespace iuic::text
