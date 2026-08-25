// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:binding;
import std;
import :type;
import :uri;
import :block.base;
import :block.static__;
import :block.dynamic__;
import :instance;

namespace iuic::external {

export struct binding;

export template <const binding &> struct self_identity {};

export struct binding {

  // CT\RT - Safe Binding
  template <const binding &self>
  constexpr binding(type::value type_, uri path_, self_identity<self> = {})
      : type{type_}, path{path_}, block_ptr{nullptr} {
    if consteval {
      block_ptr = &static_block<decltype(self_identity<self>{})>;
    } else {
      block_ptr = new dynamic_block__{};
    }
  };

  // RT - Binding
  binding(type::value type_, uri path_)
      : type{type_}, path{path_}, block_ptr{nullptr} {
    block_ptr = new dynamic_block__{};
  };

  // make conceptr is_data_type
  template <auto obj> constexpr instance make_declaration() const {
    return {type, path, block_ptr, obj};
  };

  template <typename T> instance make_declaration(T &&obj) const {
    using otype = std::remove_cvref_t<T>;
    otype *ptr = new otype{std::forward<T>(obj)};
    return instance{type, path, block_ptr, ptr};
  };

  constexpr instance make_declaration() const {
    return {type, path, block_ptr};
  };

public: // BIG-V + RAII
  binding(const binding &) = delete;
  binding(binding &&) = delete;
  // TODO : move
  binding &operator=(const binding &) = delete;
  binding &operator=(binding &&) = delete;
  constexpr ~binding() {
    if not consteval {
      if (block_ptr) {
        block_ptr->die();
      }
    }
  };

private:
  type::value type;
  uri path;
  // TODO : std::visit | union
  // отказ от virtual
  block *block_ptr{nullptr};
};
}; // namespace iuic::external
