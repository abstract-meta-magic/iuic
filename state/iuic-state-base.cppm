// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 abstract-meta-magic and contributors

export module iuic.state:base;
import :decl;

export namespace iuic::state::base {
constexpr decl hh{decl::instance_of<hh>()};

constexpr decl null{decl::instance_of<null>()};

constexpr decl hovered{decl::instance_of<hovered>()};

constexpr decl idle{decl::instance_of<idle>()};

constexpr decl exception_handling{decl::instance_of<exception_handling>()};

constexpr decl terminate{decl::instance_of<terminate>()};

constexpr decl local{decl::instance_of<local>()};

static_assert(null.self == null.self, "STATE INSTANCE IS NEQ");
static_assert(null.self != idle.self, "STATE INSTANCE IS EQ");
static_assert(null.self != hovered.self, "STATE INSTANCE IS EQ");
}; // namespace iuic::state::base
