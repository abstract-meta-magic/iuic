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

  template <typename T> bind<T> get_bind() const;

  void add_tag(tag);

  void remove_tag(tag);

  bool has_tag(tag);

  void set_reachable(bool value = true) { is_reachable_ = value; };

  bool is_reachable() const { return is_reachable_; };

  callback_handl on_reset(auto &&call);

  callback_handl on_reachable_change(auto &&call);

private:
  declaration decl_;
  type type_;
  // for bind
  std::vector<tag> tags;
  bool is_reachable_{true};
};
}; // namespace iuic::asset
