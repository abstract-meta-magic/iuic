// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:definition;
import std;
import :base;
import :declaration;
import :bind;

namespace iuic::asset {

export struct definition {
  const declaration &of() const { return decl_; };

  void set_type(type t) { type_ = t; };

  type get_type() const { return type_; };

  template <typename T> bind<T> get_bind() const { return bind<T>{bindings}; };

  template <typename T> void remove_bind() { bindings.remove<T>(); };

  void add_tag(tag t) { tags.insert(t); };

  void remove_tag(tag t) { tags.erase(t); };

  bool has_tag(tag t) const { return tags.contains(t); };

  void set_reachable(bool value = true) { is_reachable_ = value; };

  bool is_reachable() const { return is_reachable_; };

  callback_handl on_reset(auto &&call);

  callback_handl on_reachable_change(auto &&call);

  definition(declaration decl) : decl_{decl} {};

private:
  declaration decl_;
  type type_{undefined};
  std::flat_set<tag> tags;
  bool is_reachable_{true};
  mutable binding_storage bindings;
};
}; // namespace iuic::asset
