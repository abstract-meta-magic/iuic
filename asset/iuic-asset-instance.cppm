// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.asset:instance;
import std;
import :base;
import :bind;
import :definition;

namespace iuic::asset {
export struct instance {
  constexpr instance(const definition *of) : of_{of} {};

  const definition &of() const { return *of_; };
  // by default is definition.get_type()
  void set_type(type);

  type get_type() const;

  template <typename T> bind<T> get_bind() const;

  void add_tag(tag);

  void remove_tag(tag);

  bool has_tag(tag);

  // tag range ???

  bool is_reachable();

  callback_handl on_reset(auto &&call);

  callback_handl on_reachable_change(auto &&call);

private:
  const definition *of_;
  type type_;
  std::vector<tag> tags;
  // binds
};

} // namespace iuic::asset
