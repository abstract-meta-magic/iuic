// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:it.insert;
import std;
import :base;
import :underlying_table;
import :sentinel;
import :it.basic;

export namespace iuic::iterator {

template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct insert : basic<T, Mode, Ex...> {
  using basic = basic<T, Mode, Ex...>;

  insert(basic b) : basic{b} {};

  insert &operator&=(auto &&value) & {
    underlying_table::insert_at(*this, std::forward<decltype(value)>(value));
    return *this;
  };

  [[nodiscard]]
  insert operator&(auto &&value) {
    auto res{*this};
    res &= std::forward<decltype(value)>(value);
    return res;
  };

  insert &operator<<=(auto &&value) & {
    underlying_table::insert_before(*this,
                                    std::forward<decltype(value)>(value));
    return *this;
  };

  [[nodiscard]] insert operator<<(auto &&value) {
    auto res{*this};
    res <<= std::forward<decltype(value)>(value);
    return res;
  };

  insert &operator>>=(auto &&value) & {
    underlying_table::insert_after(*this, std::forward<decltype(value)>(value));
    return *this;
  };

  [[nodiscard]] insert operator>>(auto &&value) {
    auto res{*this};
    res >>= std::forward<decltype(value)>(value);
    return res;
  };

  insert &operator[](auto &&...args) & {
    underlying_table::insert_in(*this, std::forward<decltype(args)>(args)...);
    return *this;
  };

  [[nodiscard]] insert operator()(auto &&...args) {
    auto res{*this};
    res[std::forward<decltype(args)>(args)...];
    return res;
  };
};

}; // namespace iuic::iterator
