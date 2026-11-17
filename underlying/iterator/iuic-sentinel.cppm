// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:sentinel;
import std;
import :base;

namespace iuic::iterator {

template <typename T>
concept has_sentinel = requires { typename T::sentinel; };

export template <typename T, typename Mode = mode::liner_t, typename... Ex>
struct sentinel : protected underlying<T, Mode, Ex...> {

  using underlying = underlying<T, Mode, Ex...>;

  sentinel(underlying u) : underlying{u} {};

  constexpr bool operator==(const underlying &other) const {
    return this->equal(other);
  };
};

export template <typename T, typename Mode, typename... Ex>
  requires has_sentinel<underlying<T, Mode, Ex...>>
struct sentinel<T, Mode, Ex...>
    : protected underlying<T, Mode, Ex...>::sentinel {
  using underlying = underlying<T, Mode, Ex...>;

  sentinel(underlying::sentinel s) : underlying::sentinel{s} {};

  sentinel(underlying u)
    requires requires { typename underlying::sentinel{u}; }
      : underlying::sentinel{u} {};

  sentinel()
    requires requires { typename underlying::sentinel{}; }
      : underlying::sentinel{} {};

  constexpr bool operator==(const underlying &other) const {
    return other.equal(static_cast<const underlying::sentinel &>(*this));
  };
};
}; // namespace iuic::iterator
