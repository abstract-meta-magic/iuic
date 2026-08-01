// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
#pragma once

namespace iuic::cenv::__define {
// TODE RENAME ???
using size_t = unsigned long long;

struct strv {
  const char *begin;
  size_t size;
  template <size_t N>
  constexpr strv(const char (&str)[N]) : begin{str}, size{N - 1} {}

  constexpr strv(const char *str, size_t size_) : begin{str}, size{size_} {}

  constexpr bool operator==(const strv &other) const {
    if (other.size != size) {
      return false;
    } else {
      for (int i{0}; i <= size; ++i) {
        if (begin[i] != other.begin[i]) {
          return false;
        }
      }
      return true;
    }
  };
};

// TODO NORMAL ERROR MESSAGE - '\0'
static_assert(strv{"Hello World"}.size == sizeof("Hello World") - 1,
              "Bed str EQ");

enum class type { undef, str, num, logic };

union data {
  strv str;
  bool logic;
  size_t num;
};

struct decl {
  strv name;
  type type;
  data data;
};

constexpr decl undef{.name = strv{"Undefined variable"},
                     .type = type::undef,
                     .data = {.str = {""}}};

}; // namespace iuic::cenv::__define
