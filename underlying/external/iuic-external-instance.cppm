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

export struct instance {
  decltype(auto) resolve(std::invocable<resolution_context> auto &&call) const {
    return call(resolution_context{decl,
                                   block_ptr->get_resolution_segment_for(decl),
                                   block_ptr->get_general_context()});
  }; // use 'utils'{block} ???

  void reset() const {
    // TODO : IMPL
  };

  constexpr const uri &get_uri() const {
    return block_ptr->get_general_context()->uri;
  };

  constexpr type::value get_binding_type() const {
    return block_ptr->get_general_context()->type;
  }; // -> binding::type::decl
  constexpr const erasure::type *get_declaration_type() const {
    return decl->get_object().get_type();
  }; // -> erasure::type

  constexpr const erasure::type *get_resolution_type() const {
    return block_ptr->get_resolution_segment_for(decl)->object.get_type();
  }; // -> erasure::type

  constexpr bool try_visit_declaration(auto &&call) const {
    return decl->get_object().try_visit(std::forward<decltype(call)>(call));
  };

  constexpr bool try_visit_resolution(auto &&call) const {
    return erasure::visited::as_mutable{
        block_ptr->get_resolution_segment_for(decl)->object}
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

  instance fork() {
    // TODO : make fork
    // copy block
    // but new rt decl
    return {nullptr, nullptr};
  };

public: // ctor
  constexpr instance() : block_ptr{nullptr}, decl{nullptr} {}

  constexpr instance(block *block_ptr_, declaration_segment *decl_)
      : block_ptr{block_ptr_}, decl{decl_} {}

public: // BIG-V + RAII
  constexpr instance(instance &&other) : block_ptr{nullptr}, decl{nullptr} {
    std::swap(block_ptr, other.block_ptr);
    std::swap(decl, other.decl);
  }

  constexpr instance(const instance &other)
      : block_ptr{other.block_ptr}, decl{other.decl} {
    if not consteval {
      if (decl) {
        decl->retain();
      }
    }
  }

  constexpr instance &operator=(instance &&other) {
    std::swap(block_ptr, other.block_ptr);
    std::swap(decl, other.decl);

    return *this;
  }

  constexpr instance &operator=(const instance &other) {
    if not consteval {
      if (other.decl) {
        other.decl->retain();
      }
      block_ptr = other.block_ptr;
      decl = other.decl;
    }

    return *this;
  }

  constexpr ~instance() {
    if not consteval {
      if (decl) {
        decl->release();
      }
    }
  };

private:
  declaration_segment *decl;
  block *block_ptr;
};

}; // namespace iuic::external

/* TODO

instance i_1 = ...;

// общий bind
// общий decl
instance i_2 = i_1; // no-copy

// общий bind
// свой decl
instance i_3 = i_1.fork();

*/
