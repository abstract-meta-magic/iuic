// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:visited.as_mutable;
import :visited.base;

export namespace iuic::erasure {

// TODO : make contsexpr
struct visited::as_mutable : private visited {

  template <is_pure_type T>
  as_mutable(const T &data_)
      : visited{static_cast<void *>(const_cast<T *>(std::addressof(data_))),
                type::from<T>()} {};

  as_mutable(visited v) : visited{v} {};
  template <typename T>
  as_mutable(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  as_mutable(std::nullptr_t) : visited{nullptr, type::none()} {};

  result_proxy try_visit(func_as_decoy<decoy(decoy &)> auto &&call) {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    const erasure::type *otype = type::from<std::remove_cvref_t<arg_t>>();

    if (otype == type) {
      call(*static_cast<arg_ptr>(data));
      return result_proxy{true};
    }

    return result_proxy{false};
  };

  decltype(auto) unsafe_visit(auto &&call) {
    using traits = decltype(func_type{call})::traits;

    using arg_t = traits::func_args::template arg_t<0>;
    using arg_ptr = std::remove_reference_t<arg_t> *;

    if constexpr (unsafe_check) {
      const erasure::type *otype =
          erasure::type::from<std::remove_cvref_t<arg_t>>();

      if (otype != type) {
        throw std::logic_error{"visited contains other type."};
      }
    }

    return call(*static_cast<arg_ptr>(data));
  };

  auto try_visit_opt(func_as_decoy<decoy(decoy &)> auto &&call) {
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

  result_proxy try_visit_table(func_as_decoy<decoy(decoy &)> auto &&...call) {
    return result_proxy{(try_visit(std::forward<decltype(call)>(call)) || ...)};
  };

  /*
  Later.
  Call table for big callable pack size

  template <auto &table>
  auto try_visit_table(auto &&...args) {
    // table.size()
    // table.get<0..N>()
    // ...
  };

  template <auto table>
  auto try_visit_table(auto &&...args) {
    // table.size()
    // table.get<0..N>()
    // ...
  };
  */
};

}; // namespace iuic::erasure
