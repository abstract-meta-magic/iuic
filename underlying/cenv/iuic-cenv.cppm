// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module;

#include "iuic-cenv-primitive.hpp"

namespace iuic::cenv::__define {
//
#if __has_include("__iuic_cenv_define.inc")
#include "__iuic_cenv_define.inc"
#else

consteval const decl &get_decl_by_name__(const strv &) { return undef; };
constexpr bool use_env{false};
#endif
}; // namespace iuic::cenv::__define

export module iuic.underlying.cenv;
import std;
namespace iuic::cenv {

using iuic::cenv::__define::decl;
using iuic::cenv::__define::strv;
using iuic::cenv::__define::type;

consteval const decl &get_value(std::string_view name) {
  if constexpr (iuic::cenv::__define::use_env) {
    return iuic::cenv::__define::get_decl_by_name__(
        strv{name.data(), name.size()});
  } else {
    static constexpr decl undef{
        .name = strv{"???"}, .type = type::undef, .data = {.str = {""}}};
    return undef;
  }
};

}; // namespace iuic::cenv

export namespace iuic::cenv {
consteval std::optional<std::string_view> str(std::string_view name) {
  if (auto &decl = get_value(name); decl.type == type::str) {
    return std::string_view{decl.data.str.begin, decl.data.str.size};
  }
  return std::nullopt;
};

consteval std::optional<std::size_t> num(std::string_view name) {
  if (auto &decl = get_value(name); decl.type == type::num) {
    return decl.data.num;
  }
  return std::nullopt;
};

consteval std::optional<bool> logic(std::string_view name) {
  if (auto &decl = get_value(name); decl.type == type::logic) {
    return decl.data.logic;
  }
  return std::nullopt;
};
}; // namespace iuic::cenv
