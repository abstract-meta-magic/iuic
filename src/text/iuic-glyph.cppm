// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:glyph;
import std;
import iuic.underlying;

export namespace iuic::text {

struct glyph {
  units::upm width;
  units::upm height;
  units::upm advance;
  units::upm vertical_offset;
  units::upm horisontal_offset;
  using id_t = std::uint32_t;
};

}; // namespace iuic::text
