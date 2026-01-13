// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module iuic.core;
import std;
import :text.font;

namespace iuic::text {

glyph::atlas::atlas(std::unique_ptr<decoder> &&d_) : decoder_{std::move(d_)} {};
glyph::atlas::atlas(std::unique_ptr<decoder> &&d_,
                    std::unique_ptr<external::binding> &&e_)
    : decoder_{std::move(d_)}, binding{std::move(e_)} {};
}; // namespace iuic::text
