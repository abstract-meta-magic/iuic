// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.func;
import std;

export namespace iuic::func {

template <typename T> struct is_function_signature : std::false_type {};

template <typename T> struct is_pure_function_signature : std::false_type {};

template <typename... ARGS> struct func_args {
  using as_tuple_t = std::tuple<ARGS...>;
  static constexpr auto size = sizeof...(ARGS);

  template <std::size_t i>
    requires(i <= sizeof...(ARGS))
  using arg_t = std::tuple_element_t<i, as_tuple_t>;

  // type_at
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

// is_function_signature_cpt
template <typename T>
concept as_func_sig_cpt = is_function_signature<T>::value;

template <typename T>
concept is_pure_function_signature_cpt = is_pure_function_signature<T>::value;

template <typename... ARGS> struct args {
  static constexpr std::size_t size = sizeof...(ARGS);

  template <std::size_t index>
  static constexpr auto identity_at()
    requires(index < size)
  {
    return identity_at__<index, ARGS...>();
  };

  template <std::size_t index>
  using type_at = decltype(identity_at<index>())::type;

private:
  template <std::size_t counter, typename TYPE, typename... NEXT>
  static constexpr auto identity_at__() {
    if constexpr (counter > 0) {
      return identity_at__<(counter - 1), NEXT...>();
    } else {
      return std::type_identity<TYPE>{};
    };
  };
};

template <is_pure_function_signature_cpt T> struct signature {
  using value = T;

private:
  template <typename R, typename... ARGS>
  static consteval auto return__(is_pure_function_signature<R(ARGS...)>) {
    return std::type_identity<R>{};
  };

  template <typename R, typename... ARGS>
  static consteval auto args__(is_pure_function_signature<R(ARGS...)>) {
    return func::args<ARGS...>{};
  };

public:
  static consteval auto return_identity() {
    return return__(is_pure_function_signature<T>{});
  };

  using return_type = decltype(return_identity())::type;

  static consteval auto args() {
    return args__(is_pure_function_signature<T>{});
  };

  using args_type = decltype(args());
};

template <typename T> struct type;

template <typename R, typename... ARGS>
struct type<R (*)(ARGS...) noexcept(false)> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (*)(ARGS...) noexcept(false)) {};
};

template <typename R, typename... ARGS> struct type<R (*)(ARGS...) noexcept> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (*)(ARGS...) noexcept) {};
};

template <typename T, typename R, typename... ARGS>
struct type<R (T::*)(ARGS...)> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (T::*)(ARGS...)) {};
  constexpr type(auto &&) {};
};

template <typename T, typename R, typename... ARGS>
struct type<R (T::*)(ARGS...) const> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (T::*)(ARGS...) const) {};
  constexpr type(auto &&) {};
};

template <typename T, typename R, typename... ARGS>
struct type<R (T::*)(ARGS...) volatile const> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (T::*)(ARGS...) volatile const) {};
  constexpr type(auto &&) {};
};

template <typename T, typename R, typename... ARGS>
struct type<R (T::*)(ARGS...) const noexcept> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (T::*)(ARGS...) const noexcept) {};
  constexpr type(auto &&) {};
};

template <typename T, typename R, typename... ARGS>
struct type<R (T::*)(ARGS...) volatile const noexcept> {
  using signature = signature<R(ARGS...)>;
  constexpr type(R (T::*)(ARGS...) volatile const noexcept) {};
  constexpr type(auto &&) {};
};

template <typename R, typename... ARGS>
type(R (*)(ARGS...) noexcept(false)) -> type<R (*)(ARGS...) noexcept(false)>;

template <typename R, typename... ARGS>
type(R (*)(ARGS...) noexcept) -> type<R (*)(ARGS...) noexcept>;

type(auto &&lambda)
    -> type<decltype(&std::remove_cvref_t<decltype(lambda)>::operator())>;

struct nonvoid {};
struct lvalue {};
struct const_lvalue {};
struct rvalue {};
struct pvalue {};
struct pointer {};

template <typename FUNC, typename TEMPLATE>
concept func_template = true;

void test(func::func_template<nonvoid(const_lvalue, pointer)> auto &&) {};

}; // namespace iuic::func
