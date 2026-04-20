// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:glyph;
import std;
import iuic.underlying;

export namespace iuic::text {

struct glyph {
  units::upixel advance;
  using id_t = std::uint32_t;
};

struct placment {
  glyph::id_t id;
  units::upixel_l offset;
};

struct token {
  units::ui::position baseline;
  std::vector<placment> glyphs;
};

}; // namespace iuic::text
