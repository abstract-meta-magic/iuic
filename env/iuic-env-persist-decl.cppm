// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module;

#ifndef IUIC_PERSIST_MAX
#define IUIC_PERSIST_MAX 8192
#endif

export module iuic.env:persist.decl;
import std;
import iuic.underlying;

namespace iuic::environment {
constexpr std::size_t persist_max{IUIC_PERSIST_MAX};
};
