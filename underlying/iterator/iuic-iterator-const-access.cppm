// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:it.const_access;
import std;
import :base;
import :underlying_table;
import :it.basic;

export namespace iuic::iterator {

template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct const_access : private basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  const_access(basic b) : basic{b} {};

  constexpr decltype(auto) operator*() {
    return underlying_table::unwrap_as_const(*this);
  };

  constexpr decltype(auto) operator->() {
    return underlying_table::getptr_as_const(*this);
  };
};

template <typename T, typename Mode, typename... Ex>
  requires basic<T, Mode, Ex...>::underlying::allow_const_drop
struct const_access<T, Mode, Ex...> : public basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  const_access(basic b) : basic{b} {};

  constexpr decltype(auto) operator*() {
    return underlying_table::unwrap_as_const(*this);
  };

  constexpr decltype(auto) operator->() {
    return underlying_table::getptr_as_const(*this);
  };
};

template <typename T, typename Mode, typename... Ex>
const_access(basic<T, Mode, Ex...>) -> const_access<T, Mode, Ex...>;
} // namespace iuic::iterator
