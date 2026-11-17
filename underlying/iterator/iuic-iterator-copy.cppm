// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:it.copy;
import std;
import :base;
import :underlying_table;
import :sentinel;
import :it.basic;

export namespace iuic::iterator {

template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct copy : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  copy(basic b) : basic{b} {};

  constexpr decltype(auto) operator*() {
    return underlying_table::unwrap_as_const(*this);
  };
};

template <typename T, typename Mode, typename... Ex>
copy(basic<T, Mode, Ex...>) -> copy<T, Mode, Ex...>;

}; // namespace iuic::iterator
