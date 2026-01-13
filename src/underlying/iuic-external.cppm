// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying:external;
import std;
import :utils;

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
}; // namespace iuic::external
