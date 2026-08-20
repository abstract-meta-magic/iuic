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

template <typename T> struct hidden_identity__ {
  using type = T;
};

export struct binding {

  template <typename T = decltype([]() {})>
  constexpr binding(type::value type_, uri path_, hidden_identity__<T> = {})
      : type{type_}, path{path_}, block_ptr{nullptr} {
    if consteval {
      block_ptr = &static_block<T>;
    } else {
      block_ptr = new dynamic_block__{};
    }
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
