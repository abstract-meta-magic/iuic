// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:visited.as_factory;
import :visited.base;

export namespace iuic::erasure {

struct visited::as_factory : private visited {

  as_factory(visited v) : visited{v} {};
  template <typename T>
  as_factory(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  as_factory(std::nullptr_t) : visited{nullptr, type::none()} {};

  bool is_copyable() { return type->copy_ctor; };
  bool is_movable() { return type->move_ctor; };

  template <typename Alloc> visited copy(Alloc &&alloc) {
    // make assert for nullptr copy_ctor ?
    // make exception safe
    void *placement = alloc.allocate(type->size, type->align);
    if (placement) {
      type->copy_ctor(placement, data);
      return visited{placement, type};
    } else {
      return visited{nullptr};
    };
  };

  template <typename Alloc> visited move(Alloc &&alloc) {
    // make assert for nullptr move_ctor ?
    // make exception safe
    void *placement = alloc.allocate(type->size, type->align);
    if (placement) {
      type->move_ctor(placement, data);
      return visited{placement, type};
    } else {
      return visited{nullptr};
    };
  };
};
}; // namespace iuic::erasure
