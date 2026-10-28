// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:model.profile;
import std;
import :base;
import :model;

namespace iuic::text {

struct model::profile {
private:
  // make this shared
  struct unit_block {
    std::vector<metrics::cell> cell_metrics;
    std::vector<metrics::layout> layout_metrics;
    std::vector<cmid> glyph;   // Rename
    std::vector<lmid> context; // Rename
    model model;
  };

  using block_t = std::shared_ptr<unit_block>;

  profile(block_t block_) : block{std::move(block_)} {};

public:
  struct tool;

  cmid metrics_of(gid g) const { return block->glyph[std::to_underlying(g)]; };
  lmid metrics_of(gcid c) const {
    return block->context[std::to_underlying(c)];
  };
  struct model of() const { return block->model; };

  const metrics::cell &get(cmid m) const {
    return block->cell_metrics[std::to_underlying(m)];
  };

  const metrics::layout &get(lmid m) const {
    return block->layout_metrics[std::to_underlying(m)];
  };

private:
  block_t block;
};

struct model::profile::tool {
  tool(model m) : block{.model = m} {
    block.glyph.reserve(m.block->glyph.size());
    block.glyph.assign(m.block->glyph.size(), cmid::null);
    block.context.reserve(m.block->context.size());
    block.context.assign(m.block->context.size(), lmid::null);
  };

  cmid create_metrics_for_glyph(metrics::cell m = {}) {
    block.cell_metrics.push_back(m);
    return cmid{block.cell_metrics.size() - 1};
  };

  lmid create_metrics_for_context(metrics::layout m = {}) {
    block.layout_metrics.push_back(m);
    return lmid{block.layout_metrics.size() - 1};
  };

  void link_with(gid g, cmid m) { block.glyph[std::to_underlying(g)] = m; };
  void link_with(gcid c, lmid m) { block.context[std::to_underlying(c)] = m; };

  profile compile() {
    // check
    return profile{std::make_shared<unit_block>(std::move(block))};
  };

private:
  unit_block block;
};

}; // namespace iuic::text
