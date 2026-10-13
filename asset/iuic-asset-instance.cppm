// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:instance;
import std;
import :base;
import :bind;
import :definition;

namespace iuic::asset {

static constexpr type use_of;

export struct instance {
  constexpr instance(const definition *of) : of_{of} {};

  const definition &of() const { return *of_; };

  void set_type(type t) { type_ = t; };

  void reset_type() { type_ = use_of; };

  type get_type() const {
    if (type_ != use_of) {
      return type_;
    } else if (of_) {
      return of_->get_type();
    };
    return undefined;
  };

  template <typename T> bind<T> get_bind() const { return bind<T>{bindings}; };

  template <typename T> void remove_bind() { bindings.remove<T>(); };

  void add_tag(tag t) { tags.insert(t); };

  void remove_tag(tag t) { tags.erase(t); };

  bool has_tag(tag t) const { return tags.contains(t); };

  bool is_reachable() const {
    if (of_) {
      return of_->is_reachable();
    }
    return false;
  };

  callback_handl on_reset(auto &&call);

  callback_handl on_reachable_change(auto &&call);

private:
  const definition *of_;
  type type_{use_of};
  std::flat_set<tag> tags;
  mutable binding_storage bindings;
};

} // namespace iuic::asset
