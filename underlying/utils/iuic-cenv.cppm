// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.utils:cenv;
import std;
namespace iuic::cenv {

constexpr std::string_view source{""};
}; // namespace iuic::cenv

export namespace iuic::cenv {
consteval std::optional<std::string_view> str(std::string_view name) {
  return std::nullopt;
};

consteval std::optional<std::size_t> num(std::string_view name) {
  return std::nullopt;
};

consteval std::optional<bool> logic(std::string_view name) {
  return std::nullopt;
};
}; // namespace iuic::cenv
