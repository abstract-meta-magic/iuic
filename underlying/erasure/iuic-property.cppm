// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.underlying.erasure:property;
import :decl;

export namespace iuic::erasure::property {
template <is_pure_type> constexpr bool gc_allowed_for{false};

template <is_pure_type> constexpr std::size_t lifetime_for{0};
}; // namespace iuic::erasure::property
