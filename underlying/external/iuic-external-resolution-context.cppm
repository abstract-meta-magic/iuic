// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:resolution_context;
import std;
import :block.base;

namespace iuic::external {
export struct resolution_context {

  resolution_segment_proxy get_resolution() { return {resolution, general}; };

  template <typename T> shared_object_proxy<T> get_shared_object() {
    return {general};
  };

  const uri &get_uri() const { return declaration->path; };

  const type::value get_binding_type() const { return declaration->type; };

  const erasure::type *get_declaration_type() const {
    return declaration->object.get_type();
  };

  bool try_visit_declaration(auto &&call) const {
    return erasure::visited::as_const{declaration->object}.try_visit(
        std::forward<decltype(call)>(call));
  };

  resolution_context(const declaration_segment *declaration_,
                     resolution_segment *resolution_, general_context *general_)
      : declaration{declaration_}, resolution{resolution_}, general{general_} {}

private:
  const declaration_segment *declaration;
  resolution_segment *resolution;
  general_context *general;
};
} // namespace iuic::external
