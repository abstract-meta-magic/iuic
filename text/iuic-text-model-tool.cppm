// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:model.tool;
import std;
import :base;
import :model;

namespace iuic::text {

struct model::tool {
  giid create_glyph_info(glyph_info i = {}) {
    block.info.push_back(i);
    return giid{block.info.size() - 1};
  };

  miid create_meta_info(std::string_view i) {
    block.meta.push_back(std::string{i});
    return miid{block.meta.size() - 1};
  };

  cid create_compose(std::span<const gid> compose_) {
    if (compose_.empty()) {
      return cid::null;
    }

    model::unit_block::range_ range;
    range.begin = block.compose_sequences.size();
    for (auto g : compose_) {
      block.compose_sequences.push_back(g);
    };
    range.end = block.compose_sequences.size();
    block.compose.push_back(range);
    return cid{block.compose.size() - 1};
  };

  cid create_compose(std::convertible_to<gid> auto &&...compose_) {
    if constexpr (sizeof...(compose_) == 0) {
      return cid::null;
    } else {
      model::unit_block::range_ range;
      range.begin = block.compose_sequences.size();
      (block.compose_sequences.push_back(compose_), ...);
      range.end = block.compose_sequences.size();
      block.compose.push_back(range);
      return cid{block.compose.size() - 1};
    }
  };

  cid create_semantic(std::span<const struct semantic> semantic_) {
    if (semantic_.empty()) {
      return cid::null;
    }

    model::unit_block::range_ range;
    range.begin = block.semantic_sequences.size();
    for (auto g : semantic_) {
      block.semantic_sequences.push_back(g);
    };
    range.end = block.semantic_sequences.size();
    block.semantic.push_back(range);
    return cid{block.compose.size() - 1};
  };

  cid create_semantic(
      std::convertible_to<struct semantic> auto &&...semantic_) {
    if constexpr (sizeof...(semantic_) == 0) {
      return cid::null;
    } else {
      model::unit_block::range_ range;
      range.begin = block.semantic_sequences.size();
      (block.semantic_sequences.push_back(semantic_), ...);
      range.end = block.semantic_sequences.size();
      block.semantic.push_back(range);
      return cid{block.compose.size() - 1};
    }
  };

  gid create_glyph() {
    block.glyph.push_back({});
    return gid{block.glyph.size() - 1};
  };

  gcid create_context() {
    block.context.push_back({});
    return gcid{block.context.size() - 1};
  };

  // compose
  void link_with(gcid c, gid g) {
    block.context[std::to_underlying(c)].glyph = g;
  };
  void link_with(gid g, cid c) {
    block.glyph[std::to_underlying(g)].compose = c;
  };

  // info
  void link_with(gid g, giid i) {
    block.glyph[std::to_underlying(g)].info = i;
  };
  void link_with(gid g, miid m) {
    block.glyph[std::to_underlying(g)].meta = m;
  };
  void link_with(gcid c, miid m) {
    block.context[std::to_underlying(c)].meta = m;
  };

  void set_model_description(std::string_view);

  void set_model_props(properties);

  model compile() {
    return model{std::make_shared<model::unit_block>(std::move(block))};
  };

private:
  model::unit_block block;
};
}; // namespace iuic::text
