// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:type;
import :decl;
import :property;

export namespace iuic::erasure {
using dtor_fptr = void (*)(const void *const);
using deleter_fptr = void (*)(const void *const);
using copy_ctor_fptr = void (*)(void *const, const void *const);
using move_ctor_fptr = void (*)(void *const, void *const);
// can be assing copy\move

template <is_pure_type T> consteval dtor_fptr get_dtor_for() {
  return [](const void *const obj) static constexpr {
    static_cast<const T *const>(obj)->~T();
  };
};

template <is_pure_type T> consteval deleter_fptr get_deleter_for() {
  return [](const void *const obj) static constexpr {
    delete static_cast<const T *const>(obj);
  };
};

template <is_pure_type T> consteval copy_ctor_fptr get_copy_ctor_for() {
  if constexpr (std::is_copy_constructible_v<T>) {
    return [](void *const lhs, const void *const rhs) static constexpr {
      new (static_cast<T *>(lhs)) T{*static_cast<const T *>(rhs)};
    };
  } else {
    return nullptr;
  }
};

template <is_pure_type T> consteval move_ctor_fptr get_move_ctor_for() {
  if constexpr (std::is_move_constructible_v<T>) {
    return [](void *const lhs, void *const rhs) static constexpr {
      new (static_cast<T *>(lhs)) T{std::move(*static_cast<T *>(rhs))};
    };
  } else {
    return nullptr;
  }
};

struct type {
private:
  constexpr type(const void *id_, bool td, std::size_t s, std::size_t a,
                 std::size_t lt, dtor_fptr dtor_, deleter_fptr deleter_,
                 copy_ctor_fptr copy_ctor_, move_ctor_fptr move_ctor_) noexcept
      : id{id_}, trivial_dctor{td}, size{s}, align{a}, livetime{lt},
        dtor{dtor_}, deleter{deleter_}, copy_ctor{copy_ctor_},
        move_ctor{move_ctor_} {};

public:
  template <is_pure_type T> static constexpr const type *from() {

    if constexpr (requires() {
                    { T::livetime } -> std::convertible_to<std::size_t>;
                  }) {
      static constexpr type res{&res,
                                std::is_trivially_destructible_v<T>,
                                sizeof(T),
                                alignof(T),
                                T::livetime,
                                get_dtor_for<T>(),
                                get_deleter_for<T>(),
                                get_copy_ctor_for<T>(),
                                get_move_ctor_for<T>()};
      return &res;
    } else {
      static constexpr type res{&res,
                                std::is_trivially_destructible_v<T>,
                                sizeof(T),
                                alignof(T),
                                0,
                                get_dtor_for<T>(),
                                get_deleter_for<T>(),
                                get_copy_ctor_for<T>(),
                                get_move_ctor_for<T>()};
      return &res;
    }
  };

  static constexpr const type *none() { return from<std::nullptr_t>(); };

  const void *id;
  const bool trivial_dctor;
  const std::size_t size;
  const std::size_t align;
  const std::size_t livetime; //  in frames

  dtor_fptr dtor;
  deleter_fptr deleter;
  copy_ctor_fptr copy_ctor;
  move_ctor_fptr move_ctor;
  // conts *type inner...or...child
};
}; // namespace iuic::erasure
