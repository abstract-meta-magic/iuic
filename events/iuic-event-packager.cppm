// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.events:packager;
import :decl;
import :allocator;

namespace iuic::event {
template <channel ch> struct packager {
  template <is_base_event_type EVENT_TYPE>
  package make(EVENT_TYPE &&e, allocator<ch> &alloc) {
    using meta_type = std::remove_cvref_t<decltype(e.meta)>;

    auto *ptr = alloc.template allocate<meta_type>();

    new (ptr) std::remove_cvref_t<meta_type>{std::move(e.meta)};

    return package{
        .meta = ptr,
        .type = e.type,
        .call = e.call,
    };
  };
};
}; // namespace iuic::event
