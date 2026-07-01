// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:visited.as_const;
import :visited.base;

export namespace iuic::erasure {

struct visited::as_const : private visited {
  template <is_pure_type T>
  constexpr as_const(const T &data_)
      : visited{static_cast<void *>(const_cast<T *>(std::addressof(data_))),
                type::from<T>()} {};
  constexpr as_const(visited v) : visited{v} {};
  template <is_pure_type T>
  constexpr as_const(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  constexpr as_const(std::nullptr_t) : visited{nullptr, type::none()} {};

  constexpr bool
  try_visit(func_as_decoy<decoy(const decoy &)> auto &&call) const {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    const erasure::type *otype = type::from<std::remove_cvref_t<arg_t>>();

    if (otype == type) {
      call(*static_cast<arg_ptr>(data));
      return true;
    }

    return false;
  };

  decltype(auto) unsafe_visit(auto &&call) const {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    using ret_type = std::optional<decltype(call(*static_cast<arg_ptr>(data)))>;

    if constexpr (unsafe_check) {
      const erasure::type *otype =
          erasure::type::from<std::remove_cvref_t<arg_t>>();

      if (otype != type) {
        throw std::logic_error{"visited contains other type."};
      }
    }

    return call(*static_cast<arg_ptr>(data));
  };

  auto try_visit_opt(func_as_decoy<decoy(const decoy &)> auto &&call) const {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    using ret_type = std::optional<decltype(call(*static_cast<arg_ptr>(data)))>;

    const erasure::type *otype = type::from<std::remove_cvref_t<arg_t>>();

    if (otype == type) {
      return ret_type{call(*static_cast<arg_ptr>(data))};
    } else {
      return ret_type{std::nullopt};
    }
  }

  bool visit_any(func_as_decoy<decoy(const decoy &)> auto &&...calls) const {
    return (try_visit(std::forward<decltype(calls)>(calls)) || ...);
  };

  decltype(auto) visit_or(func_as_decoy<decoy(const decoy &)> auto &&call,
                          std::invocable<> auto call_or);
};

}; // namespace iuic::erasure
