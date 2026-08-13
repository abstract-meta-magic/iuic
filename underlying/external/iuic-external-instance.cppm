// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:instance;
import std;
import :type;
import :uri;
import :block.base;

namespace iuic::external {

static constexpr type::decl null_extern_type__;

export struct instance : private native_instance {
  constexpr bool is_resolved() const { return false; };
  bool resolve(auto ctx) const { return false; }; // use 'utils'{block} ???
  void reset() const {};

  constexpr const uri &get_uri() const { return path; };

  constexpr type::value get_binding_type() const {
    return type;
  }; // -> binding::type::decl
  constexpr const erasure::type *get_declaration_type() const {
    return declaration.get_type();
  }; // -> erasure::type
  constexpr const erasure::type *get_resolution_type() const {
    return nullptr;
  }; // -> erasure::type
  constexpr bool try_visit_declaration(auto &&call) const {
    return erasure::visited::as_const{declaration}.try_visit(
        std::forward<decltype(call)>(call));
  };
  constexpr bool try_visit_resolution(auto) const { return false; };

public: // ctor
  constexpr instance()
      : native_instance{null_extern_type__, uri{"", ""}}, block_ptr{nullptr} {}

  constexpr instance(type::value type_, uri path_, block *block_ptr_)
      : native_instance{type_, path_}, block_ptr{block_ptr_} {}

  template <typename T, typename UNIQUE = decltype([]() {})>
  constexpr instance(type::value type_, uri path_, block *block_ptr_, T &&obj)
      : native_instance{type_, path_, obj}, block_ptr{block_ptr_} {}

  template <typename T, typename UNIQUE = decltype([]() {})>
  instance(type::value type_, uri path_, block *block_ptr_, T *obj)
      : native_instance{type_, path_, obj}, block_ptr{block_ptr_} {}

public: // BIG-V + RAII
  constexpr instance(instance &&other)
      : native_instance{other}, block_ptr{other.block_ptr} {}

  constexpr instance(const instance &other)
      : native_instance{other}, block_ptr{other.block_ptr} {}

  instance &operator=(instance &&other) {
    std::swap(type, other.type);
    std::swap(declaration, other.declaration);
    std::swap(path, other.path);
    std::swap(block_ptr, other.block_ptr);

    return *this;
  }

  constexpr ~instance() {
    if not consteval {
      /* this mb leak
    if (declaration.get_type() != nullptr) {
      erasure::visited::as_garbage{declaration}.destruct();
      erasure::visited::as_garbage{declaration}.free();
    }
      */
    }
  };

private:
  block *block_ptr;
};

}; // namespace iuic::external
