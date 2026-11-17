// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.iterator:underlying_table;
import std;
import :base;

namespace iuic::iterator::underlying_table {

template <typename T>
concept has_unwrap = requires(T obj) { obj.unwrap_as_mutable(); };

template <typename T>
concept has_const_unwrap = requires(T obj) { obj.unwrap_as_const(); };

template <typename T>
concept has_getptr = requires(T obj) { obj.getptr_as_mutable(); };

template <typename T>
concept has_const_getptr = requires(T obj) { obj.getptr_as_const(); };

template <typename T>
concept has_increment = requires(T obj) { obj.increment(); };

template <typename T>
concept has_decrement = requires(T obj) { obj.decrement(); };

template <typename L, typename R>
concept has_assign = requires(L &lhs, const R &rhs) { lhs.assign(rhs); };

template <typename T, typename M, typename... Ex>
  requires has_unwrap<underlying<T, M, Ex...>>
constexpr decltype(auto)
unwrap_as_mutable(underlying<T, M, Ex...> &underlying) {
  return underlying.unwrap_as_mutable();
};

template <typename T, typename M, typename... Ex>
  requires has_const_unwrap<underlying<T, M, Ex...>>
constexpr decltype(auto) unwrap_as_const(underlying<T, M, Ex...> &underlying) {
  return underlying.unwrap_as_const();
};

template <typename T, typename M, typename... Ex>
  requires has_increment<underlying<T, M, Ex...>>
constexpr decltype(auto) increment(underlying<T, M, Ex...> &underlying) {
  return underlying.increment();
};

template <typename T, typename M, typename... Ex>
  requires has_decrement<underlying<T, M, Ex...>>
constexpr decltype(auto) decrement(underlying<T, M, Ex...> &underlying) {
  return underlying.decrement();
};

template <typename T, typename M, typename... Ex>
  requires has_const_getptr<underlying<T, M, Ex...>>
constexpr decltype(auto) getptr_as_const(underlying<T, M, Ex...> &underlying) {
  return underlying.getptr_as_const();
};

template <typename T, typename M, typename... Ex>
  requires has_getptr<underlying<T, M, Ex...>>
constexpr decltype(auto)
getptr_as_mutable(underlying<T, M, Ex...> &underlying) {
  return underlying.getptr_as_mutable();
};

template <typename T, typename M, typename... Ex, typename OC, typename OM,
          typename... OEx>
  requires has_assign<underlying<T, M, Ex...>, underlying<OC, OM, OEx...>>
constexpr decltype(auto) assign(underlying<T, M, Ex...> &lsh,
                                const underlying<OC, OM, OEx...> &rhs) {
  return lsh.assign(rhs);
};

// TODO: concept's
template <typename T, typename M, typename... Ex>
constexpr void insert_at(underlying<T, M, Ex...> &underlying, auto &&...args) {
  underlying.insert_at(std::forward<decltype(args)>(args)...);
};

template <typename T, typename M, typename... Ex>
constexpr void insert_in(underlying<T, M, Ex...> &underlying, auto &&...args) {
  underlying.insert_in(std::forward<decltype(args)>(args)...);
};

template <typename T, typename M, typename... Ex>
constexpr void insert_before(underlying<T, M, Ex...> &underlying,
                             auto &&...args) {
  underlying.insert_before(std::forward<decltype(args)>(args)...);
};

template <typename T, typename M, typename... Ex>
constexpr void insert_after(underlying<T, M, Ex...> &underlying,
                            auto &&...args) {
  underlying.insert_after(std::forward<decltype(args)>(args)...);
};

}; // namespace iuic::iterator::underlying_table
