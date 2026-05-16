// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:packager;
import :decl;
import :allocator;

export namespace iuic::event {
template <const channel &ch> struct packager {
  template <is_base_event_type EVENT_TYPE>
  package make(EVENT_TYPE &&e, allocator<ch> &alloc) {

    // TODO : подумать о том
    // что call может быть обычным вызовом
    // или лямбдой\функтором
    // в таком случае его нужно будет
    // упаковать

    if constexpr (requires { e.meta; }) {
      using meta_type = std::remove_cvref_t<decltype(e.meta)>;
      auto *ptr = alloc.template allocate<meta_type>();

      new (ptr) std::remove_cvref_t<meta_type>{std::move(e.meta)};
      return package{
          .meta = ptr,
          .type = e.type,
          .call = e.call,
      };
    } else {
      return package{
          .meta = nullptr,
          .type = e.type,
          .call = e.call,
      };
    }
  };
};
}; // namespace iuic::event
