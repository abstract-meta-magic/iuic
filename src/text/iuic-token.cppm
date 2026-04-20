// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:token;
import std;
import iuic.underlying;
import :glyph;

export namespace iuic::text {

namespace raw {
struct token {
  std::size_t atlas_id;
  std::vector<glyph::id_t> glyphs;
};
}; // namespace raw
namespace present {

struct placement {
  glyph::id_t id;
  units::pixel offset;
};

struct token {
  std::size_t atlas_id;
  float scale;
  units::ui::position baseline;
  std::span<const placement> placement;
};
}; // namespace present
}; // namespace iuic::text
