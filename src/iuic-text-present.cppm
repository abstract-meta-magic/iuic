// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.core:text.present;
import std;
import iuic.underlying;
import :text.token;
import :text.font;

export namespace iuic::text {

struct present_node : token {
  units::ui::rect rect;
  //..
};

struct present {
  class aggregator;
  std::vector<present_node> nodes;
};

}; // namespace iuic::text

namespace iuic::text {

struct aggregation_bondle {
  token::sequence tokens{};
  glyph::sequence glyphs;
};

// frame-type
class present::aggregator {
public:
  // подготовка места для present, под token::sequence
  void reserve_present(std::size_t id, token::sequence sq);

  void apply_present(std::size_t id, glyph::sequence present);

  token::sequence get_tokens(std::size_t id) const;

  glyph::sequence get_present(std::size_t id) const;

private:
  std::map<std::size_t, aggregation_bondle> presents;
};
}; // namespace iuic::text
