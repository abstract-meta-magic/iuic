// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:visited.as_garbage;
import :visited.base;

export namespace iuic::erasure {
struct visited::as_garbage : private visited {
  constexpr as_garbage(visited v) : visited{v} {};
  template <typename T>
  constexpr as_garbage(T *data_) : visited{data_, type::from<pure_t<T>>()} {};

  constexpr as_garbage(std::nullptr_t) : visited{nullptr, type::none()} {};

  constexpr void free() const { type->deleter(data); };

  constexpr void destruct() const { type->dtor(data); };
};

}; // namespace iuic::erasure
