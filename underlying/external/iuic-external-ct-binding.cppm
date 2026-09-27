// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:ct_binding;
import iuic.underlying.utils;
import :uri;
import :type;

export namespace iuic::external {
template <const type::decl &type_, utils::ct::str str> struct ct_binding {
  static constexpr auto &type = type_;
  static constexpr uri uri{str.data};
};
}; // namespace iuic::external
