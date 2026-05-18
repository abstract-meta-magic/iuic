// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:decl;
import std;

namespace iuic::erasure {

template <typename T> consteval auto remove_all_pointer() {
  if constexpr (std::is_pointer_v<T>) {
    return remove_all_pointer<std::remove_pointer_t<T>>();
  } else {
    return std::type_identity<T>{};
  }
}

template <typename T> consteval decltype(auto) get_pure_type__() {
  return int{};
};
} // namespace iuic::erasure

export namespace iuic::erasure {

template <typename T>
using pure_t =
    std::remove_cvref_t<typename decltype(remove_all_pointer<T>())::type>;

template <typename T> using as_pure_type = decltype(get_pure_type__<T>());

template <typename T>
concept is_pure_type =
    std::same_as<std::remove_cvref_t<T>, T> && not std::is_pointer_v<T>;

template <typename T> struct is_function_signature : std::false_type {};

template <typename T> struct is_pure_function_signature : std::false_type {};

template <typename... ARGS> struct func_args {
  using as_tuple_t = std::tuple<ARGS...>;
  static constexpr auto size = sizeof...(ARGS);

  template <std::size_t i>
    requires(i <= sizeof...(ARGS))
  using arg_t = std::tuple_element_t<i, as_tuple_t>;
};

template <typename R, typename... ARGS>
struct is_function_signature<R(ARGS...)> : std::true_type {};

// cv cvalifier
template <typename R, typename... ARGS>
struct is_pure_function_signature<R(ARGS...)> : std::true_type {};

template <typename R, typename... ARGS>
struct is_function_signature<R (*)(ARGS...)>
    : is_function_signature<R(ARGS...)> {};

template <typename R, typename... ARGS>
struct is_function_signature<R (&)(ARGS...)>
    : is_function_signature<R(ARGS...)> {};

template <typename R, typename T, typename... ARGS>
struct is_function_signature<R (T::*)(ARGS...)>
    : is_function_signature<R(ARGS...)> {};

template <typename R, typename T, typename... ARGS>
struct is_function_signature<R (T::*)(ARGS...) const>
    : is_function_signature<R(ARGS...)> {};

template <typename T>
concept as_func_sig_cpt = is_function_signature<T>::value;

template <as_func_sig_cpt T> struct func_traits;

template <typename R, typename... ARGS> struct func_traits<R(ARGS...)> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;
};

template <typename R, typename... ARGS> struct func_traits<R (*)(ARGS...)> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;
};

template <typename R, typename T, typename... ARGS>
struct func_traits<R (T::*)(ARGS...)> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;
};

// TODO : Try support [int*&,int****&,int**&,int***] ref to pointer, pointret to
// pointer
template <typename T, typename D> struct like {
  static constexpr bool value =
      ((std::is_lvalue_reference_v<T> && std::is_lvalue_reference_v<D>) ||
       (std::is_rvalue_reference_v<T> && std::is_rvalue_reference_v<D>) ||
       (is_pure_type<std::remove_const_t<std::remove_volatile_t<T>>> &&
        is_pure_type<std::remove_const_t<std::remove_volatile_t<D>>>) ||
       (std::is_pointer_v<T> && std::is_pointer_v<D>)) &&
      (std::is_const_v<std::remove_volatile_t<std::remove_reference_t<T>>> ==
       std::is_const_v<std::remove_volatile_t<std::remove_reference_t<D>>>) &&
      (std::is_volatile_v<std::remove_const_t<std::remove_reference_t<T>>> ==
       std::is_volatile_v<std::remove_const_t<std::remove_reference_t<D>>>);
};

// TODO : other as_decoy methods for traits
template <typename R, typename T, typename... ARGS>
struct func_traits<R (T::*)(ARGS...) const> {
  using signature_t = R(ARGS...);
  using return_t = R;
  using func_args = func_args<ARGS...>;
  static constexpr bool has_return_value = not std::same_as<R, void>;

  template <as_func_sig_cpt U> static constexpr bool as_decoy() {
    using other_trait = func_traits<U>;

    if constexpr (like<typename other_trait::return_t, return_t>::value) {

      if constexpr (other_trait::func_args::size == func_args::size) {
        auto f = []<typename... SELF_ARGS, typename... OTHER_ARGS>(
                     erasure::func_args<SELF_ARGS...>,
                     erasure::func_args<OTHER_ARGS...> other) static {
          return (like<SELF_ARGS, OTHER_ARGS>::value && ...);
        };

        return f(func_args{}, typename other_trait::func_args{});
      }
    }

    return false;
  };
};

struct decoy {};

template <as_func_sig_cpt T> struct func_type {
  using traits = func_traits<T>;

  constexpr func_type(T) {};

  constexpr func_type(auto &&heh) {};

  constexpr func_type() {};

  constexpr bool operator==(const func_type &) const noexcept { return true; }

  template <as_func_sig_cpt Other>
  constexpr bool operator==(const func_type<Other> &) const noexcept {
    return std::same_as<typename traits::signature_t,
                        typename func_type<Other>::traits::signature_t>;
  }
};

func_type(auto t) -> func_type<decltype(&decltype(t)::operator())>;

template <func_type ft> struct funt_type_t {
  using traits = typename decltype(ft)::traits;
};

template <typename T, typename D>
concept func_as_decoy = requires(T call) {
  { func_type{call} };
  requires decltype(func_type{call})::traits::template as_decoy<D>();
};

template <typename T, typename D>
concept func_as_strong = requires(T call, D strong) {
  { func_type{call} };
  { func_type<D>{} };
  requires decltype(func_type{call}){} == func_type<D>{};
};

struct vector {
  // erasure vecotr
  // move -> visited|free_space|data <- move
  // TODO : someday
};
}; // namespace iuic::erasure
