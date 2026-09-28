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

template <typename T> constinit T civalue{};
template <auto v> constinit decltype(v) ivalue{v};

export struct binding {
  template <const type::decl &type, utils::ct::str str,
            utils::unique_type unique = {{}}>
  consteval binding(ct_binding<type, str> info) : block_ptr{nullptr} {
    block_ptr = &static_block<decltype(unique), decltype(info)>;
  };

  // RT - Binding
  binding(type::value type_, uri path_) : block_ptr{nullptr} {
    block_ptr = new dynamic_block__{type_.decl, path_};
  };

  template <auto obj, utils::unique_type = {{}}>
  consteval instance make_ct_instance() const {
    struct _ : declaration_segment {
      erasure::visited::as_const get_object() override {
        return erasure::visited{&ivalue<obj>};
      };
      void retain() override {};
      void release() override {};
    };
    return {block_ptr, &civalue<_>};
  };

  template <utils::unique_type = {{}}>
  consteval instance make_ct_instance() const {
    struct _ : declaration_segment {
      erasure::visited::as_const get_object() override { return nullptr; };
      void retain() override {};
      void release() override {};
    };

    return {block_ptr, &civalue<_>};
  };

  instance make_instance() const {
    struct _ : declaration_segment {
      erasure::visited::as_const get_object() override { return nullptr; };
      void retain() override {};
      void release() override {};
    };

    return {block_ptr, &civalue<_>};
  };

  template <typename T> instance make_instance(T &&obj) const {
    using type = std::remove_cvref_t<T>;

    // TODO : thread unsafe
    struct _ : declaration_segment {
      erasure::visited::as_const get_object() override {
        return erasure::visited::as_const{obj};
      };
      void retain() override { ++counter; };
      void release() override {
        if (--counter < 1) {
          delete this;
        }
      };
      constexpr _(decltype(std::forward<T>(obj)) obj_)
          : obj{std::forward<T>(obj_)} {}
      type obj;
      std::atomic_int64_t counter{1};
    };

    auto *ptr = ::new _{std::forward<T>(obj)};

    return {block_ptr, ptr};
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
        block_ptr->release();
      }
    }
  };

private:
  block *block_ptr{nullptr};
};
}; // namespace iuic::external
