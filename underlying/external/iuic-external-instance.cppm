// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:instance;
import std;
import :type;
import :uri;
import :block.base;
import :resolution_context;

namespace iuic::external {

static constexpr type::decl null_extern_type__;

export struct instance : private declaration_segment {
  decltype(auto) resolve(std::invocable<resolution_context> auto &&call) const {
    return call(resolution_context{this,
                                   block_ptr->get_resolution_segment_for(this),
                                   block_ptr->get_general_context()});
  }; // use 'utils'{block} ???

  void reset() const {};

  constexpr const uri &get_uri() const { return path; };

  constexpr type::value get_binding_type() const {
    return type;
  }; // -> binding::type::decl
  constexpr const erasure::type *get_declaration_type() const {
    return object.get_type();
  }; // -> erasure::type
  constexpr const erasure::type *get_resolution_type() const {
    return block_ptr->get_resolution_segment_for(this)->object.get_type();
  }; // -> erasure::type
  constexpr bool try_visit_declaration(auto &&call) const {
    return erasure::visited::as_const{object}.try_visit(
        std::forward<decltype(call)>(call));
  };

  constexpr bool try_visit_resolution(auto &&call) const {
    return erasure::visited::as_mutable{
        block_ptr->get_resolution_segment_for(this)->object}
        .try_visit(std::forward<decltype(call)>(call));
  };

  constexpr bool try_visit_shared_object(auto &&call) const {
    using func_type = decltype(iuic::func::type{call});

    using func_args = func_type::signature::args_type;

    static_assert(func_args::size == 1, "invalid arg size");

    using arg_type = func_args::template type_at<0>;

    static_assert(std::is_lvalue_reference_v<arg_type>,
                  "make shared rq pointer type");

    using pure_type = std::remove_reference_t<arg_type>;

    if (auto *ptr = block_ptr->get_general_context()->get_shared<pure_type>()) {
      call(*ptr);
      return true;
    }
    return false;
  };

  // try_visit_shared(...)

public: // ctor
  constexpr instance()
      : declaration_segment{null_extern_type__, uri{"", ""}},
        block_ptr{nullptr} {}

  constexpr instance(type::value type_, uri path_, block *block_ptr_)
      : declaration_segment{type_, path_}, block_ptr{block_ptr_} {}

  template <typename T, typename UNIQUE = decltype([]() {})>
  constexpr instance(type::value type_, uri path_, block *block_ptr_, T &&obj)
      : declaration_segment{type_, path_, obj}, block_ptr{block_ptr_} {}

  template <typename T, typename UNIQUE = decltype([]() {})>
  instance(type::value type_, uri path_, block *block_ptr_, T *obj)
      : declaration_segment{type_, path_, obj}, block_ptr{block_ptr_} {}

public: // BIG-V + RAII
  constexpr instance(instance &&other)
      : declaration_segment{other}, block_ptr{other.block_ptr} {}

  constexpr instance(const instance &other)
      : declaration_segment{other}, block_ptr{other.block_ptr} {}

  instance &operator=(instance &&other) {
    std::swap(type, other.type);
    std::swap(object, other.object);
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
