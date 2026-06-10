// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.query;
import std;
import iuic.underlying.erasure;

export namespace iuic::query {

struct type {};

struct tag {
  // TODO : can add available type list
};

template <typename T>
concept is_query_type =
    std::is_base_of_v<type, T> ||
    std::same_as<typename std::remove_cvref_t<T>::type_tag, type>;

template <typename T>
concept is_query_tag =
    std::is_base_of_v<tag, T> ||
    std::same_as<typename std::remove_cvref_t<T>::type_tag, tag>;

template <is_query_tag T, is_query_type... Ts> struct expr {

  constexpr expr() {};

  // TODO : make normal copy\move\forward\cat

  template <typename New, is_query_type... OTs>
  constexpr expr(expr<T, OTs...> &&other, New &&v)
      : expr{make_expr(std::move(other), std::forward<New>(v))} {};

  template <typename New, is_query_type... OTs>
  constexpr expr(const expr<T, OTs...> &other, New &&v)
      : expr{make_expr(other, std::forward<New>(v))} {};

  template <std::size_t index> constexpr const auto &at() const {
    return std::get<index>(value);
  };

  constexpr bool unroll(auto &&call) {
    return [&]<std::size_t... I>(std::index_sequence<I...>) {
      return (call(at<I>()) | ...);
    }(index_sequence());
  };

  template <typename TAG, typename... ARGS>
  constexpr expr(TAG, ARGS &&...args) : value{std::forward<ARGS>(args)...} {}

  static constexpr std::size_t size() { return sizeof...(Ts); };

  static constexpr auto index_sequence() {
    return std::make_index_sequence<size()>{};
  };

private:
  template <typename New, typename... OTs>
  static constexpr auto make_expr(expr<T, OTs...> other, New &&n) {
    if constexpr (sizeof...(OTs) == 0) {
      return expr{T{}, std::forward<New>(n)};
    } else {
      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return expr{T{}, std::get<I>(other.value)..., std::forward<New>(n)};
      }(std::make_index_sequence<sizeof...(OTs)>{});
    }
  };
  template <is_query_tag, is_query_type...> friend struct expr;
  std::tuple<Ts...> value;
};

template <typename T, typename New, typename... Ts>
expr(expr<T, Ts...> &&, New &&) -> expr<T, Ts..., New>;

template <typename T, typename New, typename... Ts>
expr(const expr<T, Ts...> &, New &&) -> expr<T, Ts..., New>;

template <typename TAG, typename... ARGS>
expr(TAG, ARGS &&...) -> expr<TAG, ARGS...>;

template <is_query_type... Ts, is_query_tag T, is_query_type New>
constexpr auto operator|(expr<T, Ts...> lhs, New &&rhs) {
  return expr<T, Ts..., New>{std::move(lhs), std::forward<New>(rhs)};
}

template <is_query_type EXPR> struct qnot {
  using type_tag = type;
  EXPR expr;
};

template <erasure::is_pure_type T> struct qtype_t : type {
  using type = T;
};

template <erasure::is_pure_type T> constexpr qtype_t<T> qtype;

/* in later versions
template <is_query_type... EXPR> struct qor {
std::tuple<EXPR...> expr;
};

struct branch {
std::tuple<EXPR...> expr;
// bind
};
*/
}; // namespace iuic::query
