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
  float scale;
  std::vector<glyph::id_t> meta;
};
}; // namespace raw
namespace present {

struct placement {
  glyph::id_t id;
  units::ui::local_position position;
};

struct token {
  std::size_t atlas_id;
  float scale;
  units::ui::position position;
  std::vector<placement> placement;
};
}; // namespace present
}; // namespace iuic::text
