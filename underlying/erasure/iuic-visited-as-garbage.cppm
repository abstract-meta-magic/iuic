// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:visited.as_garbage;
import :visited.base;

export namespace iuic::erasure {
struct visited::as_garbage : private visited {
  as_garbage(visited v) : visited{v} {};
  template <typename T>
  as_garbage(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  as_garbage(std::nullptr_t) : visited{nullptr, type::none()} {};

  void free() { type->deleter(data); };

  void destruct() { type->dtor(data); };
};

}; // namespace iuic::erasure
