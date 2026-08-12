// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:packager;
import :decl;
import :allocator;

export namespace iuic::event {

template <typename T>
concept is_packager = true;

template <const channel &ch> struct packager {
  static_assert(false, "For this channel packager is not defined. Pleas watch "
                       "docs [iuic.event.packager]");
};
}; // namespace iuic::event
