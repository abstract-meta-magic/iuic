// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying:erasure;
import std;

namespace iuic::erasure {

template <typename T> consteval auto remove_all_pointer() {
  if constexpr (std::is_pointer_v<T>) {
    return remove_all_pointer<std::remove_pointer_t<T>>();
  } else {
    return std::type_identity<T>{};
  }
}
} // namespace iuic::erasure

export namespace iuic::erasure {

template <typename T>
using pure_t =
    std::remove_cvref_t<typename decltype(remove_all_pointer<T>())::type>;

template <typename T>
concept as_pure_type =
    std::same_as<std::remove_cvref_t<T>, T> && not std::is_pointer_v<T>;

// advanced tech
using ctor_fptr_t = void (*)(void *memory);
using copy_ctor_fptr_t = void (*)(void *memory, void *object);
using move_ctor_fptr_t = void (*)(void *memory, void *object);
using copy_assign_fptr_t = void (*)(void *lhs, void *rhs);
using move_assign_fptr_t = void (*)(void *lhs, void *rhs);

template <typename T> ctor_fptr_t ctor_for() {
  return [](void *memory) static { new (memory) T{}; };
};

template <typename T> copy_ctor_fptr_t copy_ctor_for() {
  return [](void *memory, void *object) static {
    new (memory) T{*static_cast<T *>(object)};
  };
};

template <typename T> copy_ctor_fptr_t move_ctor_for() {
  return [](void *memory, void *object) static {
    new (memory) T{std::move(*static_cast<T *>(object))};
  };
};

template <typename T> copy_ctor_fptr_t copy_assign_for() {
  return [](void *lhs, void *rhs) static {
    *static_cast<T *>(lhs) = *static_cast<T *>(rhs);
  };
};

template <typename T> copy_ctor_fptr_t move_assign_for() {
  return [](void *lhs, void *rhs) static {
    *static_cast<T *>(lhs) = std::move(*static_cast<T *>(rhs));
  };
};

// using args_ctor_fptr_t = void(void*memory,/* args ? */);

struct type {
  template <as_pure_type T> static const type *from() {

    if constexpr (requires() {
                    { T::livetime } -> std::convertible_to<std::size_t>;
                  }) {
      static constexpr type res{&res,
                                std::is_trivially_destructible_v<T>,
                                sizeof(T),
                                alignof(T),
                                T::livetime,
                                [](const void *const obj) static {
                                  delete static_cast<const T *const>(obj);
                                }};
      return &res;
    } else {
      static constexpr type res{&res,
                                std::is_trivially_destructible_v<T>,
                                sizeof(T),
                                alignof(T),
                                0,
                                [](const void *const obj) static {
                                  delete static_cast<const T *const>(obj);
                                }};
      return &res;
    }
  };

  static const type *none() {
    struct _ {};
    return from<_>();
  };

  const void *const id;
  const bool trivial_dctor;
  const std::size_t size;
  const std::size_t align;
  const std::size_t livetime; //  in frames
  void (*const dctor)(const void *const);

private:
  constexpr type(const void *const i, bool td, std::size_t s, std::size_t a,
                 std::size_t lt, void (*const d)(const void *const)) noexcept
      : id{i}, trivial_dctor{td}, size{s}, align{a}, livetime{lt}, dctor{d} {};
};

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
       (as_pure_type<std::remove_const_t<std::remove_volatile_t<T>>> &&
        as_pure_type<std::remove_const_t<std::remove_volatile_t<D>>>) ||
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

struct visited {
  struct as_const;
  struct as_const_sync;
  struct as_mutable;
  struct as_mutable_sync;

  template <typename T> bool as() const noexcept;

  bool as(const type *) const noexcept;

  visited(const void *data_, const type *type_)
      : data{const_cast<void *>(data_)}, type{type_} {};

private:
  void *data;
  const type *type;
};

struct visited::as_const : private visited {
  as_const(visited v) : visited{v} {};
  template <typename T>
  as_const(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  as_const(std::nullptr_t) : visited{nullptr, type::none()} {};

  bool try_visit(func_as_decoy<decoy(const decoy &)> auto &&call) {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    if (type::from<pure_t<arg_t>>() == type) {
      call(*static_cast<arg_ptr>(data));
      return true;
    }
    return false;
  };

  bool visit_any(func_as_decoy<decoy(const decoy &)> auto &&...calls) {
    return (try_visit(std::forward<decltype(calls)>(calls)) || ...);
  };

  decltype(auto) visit_or(func_as_decoy<decoy(const decoy &)> auto &&call,
                          std::invocable<> auto call_or);
};

struct visited::as_mutable : private visited {
  as_mutable(visited v) : visited{v} {};
  template <typename T>
  as_mutable(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  as_mutable(std::nullptr_t) : visited{nullptr, type::none()} {};

  bool try_visit(func_as_decoy<decoy(decoy &)> auto &&call) {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    if (type::from<pure_t<arg_t>>() == type) {
      call(*static_cast<arg_ptr>(data));
      return true;
    }

    return false;
  };

  decltype(auto) visit_or(auto, auto);

  visited::as_const as_const() const { return visited{*this}; };
};

void hehh(visited::as_mutable m) {
  m.try_visit([](std::vector<int> &vec) {
    // job
  });
}
}; // namespace iuic::erasure
