// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:it.access;
import std;
import :base;
import :underlying_table;
import :it.basic;

export namespace iuic::iterator {

template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct access : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  access(const basic &b) : basic{b} {};
  access(basic &&b) : basic{std::move(b)} {};

  template <typename Other, typename OMode, typename... OEx>
  access(iterator::basic<Other, OMode, OEx...> other)
      : basic{std::move(other)} {};

  constexpr decltype(auto) operator*() {
    return underlying_table::unwrap_as_mutable(*this);
  };

  constexpr decltype(auto) operator->() {
    return underlying_table::getptr_as_mutable(*this);
  }
};

template <typename T, typename Mode, typename... Ex>
access(basic<T, Mode, Ex...>) -> access<T, Mode, Ex...>;

}; // namespace iuic::iterator
