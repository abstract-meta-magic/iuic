// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:it.move;
import std;
import :base;
import :underlying_table;
import :sentinel;
import :it.basic;

export namespace iuic::iterator {

template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct move : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  move(basic b) : basic{b} {};

  constexpr decltype(auto) operator*() {
    return std::move(underlying_table::unwrap_as_mutable(*this));
  };
};

template <typename T, typename Mode, typename... Ex>
move(basic<T, Mode, Ex...>) -> move<T, Mode, Ex...>;

}; // namespace iuic::iterator
