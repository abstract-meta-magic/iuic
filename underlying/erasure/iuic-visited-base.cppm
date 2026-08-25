// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:visited.base;
import iuic.underlying.cenv;
import :decl;
import :type;

export namespace iuic::erasure {
struct visited {
  // TODO : replace to env or cenv or macros
  static constexpr bool unsafe_check{
      not iuic::cenv::logic("iuic::runtime.unsafe").value_or(false)};
  struct as_const;
  struct as_const_sync;
  struct as_mutable;
  struct as_mutable_sync;
  struct as_garbage;
  struct as_factory;

  template <is_pure_type T> bool is() const noexcept {
    return type::from<T>() == type;
  };

  constexpr bool is(const type *t) const noexcept { return t == type; };

  constexpr const type *get_type() const noexcept { return type; };

  constexpr const void *raw() const noexcept { return data; };

  constexpr void *raw() noexcept { return data; };

  template <is_pure_type T>
  constexpr visited(const T &data_)
      : data{static_cast<void *>(const_cast<T *>(std::addressof(data_)))},
        type{type::from<T>()} {};
  // TODO : add ctor from rvalue is bad idia ?

  template <is_pure_type T>
  constexpr visited(const T *data_)
      : data{static_cast<void *>(const_cast<T *>(data_))},
        type{type::from<T>()} {};

  constexpr visited(const void *data_, const type *type_)
      : data{const_cast<void *>(data_)}, type{type_} {};

  constexpr visited(std::nullptr_t) : data{nullptr}, type{type::none()} {};

private:
  void *data;
  const type *type;
};

}; // namespace iuic::erasure
