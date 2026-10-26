// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:present.tool;
import iuic.underlying;
import std;
import :base;
import :present;

namespace iuic::text {

struct present::tool {
  struct placement_proxy;
  struct expansion_proxy {
    friend tool::placement_proxy;
    expansion_proxy(tool *owner_, std::size_t index_)
        : owner{owner_}, index{index_} {};

    void append(gid g) { owner->expansions[index].seq.push_back(g); };

  private:
    tool *owner;
    std::size_t index;
  };

  struct placement_proxy {
    placement_proxy(tool *owner_, std::size_t index_)
        : owner{owner_}, index{index_} {};
    void append(gid g, units::pixel offset) {
      owner->placments[index].seq.push_back(placed{g, offset});
    };

    void link_with(expansion_proxy ex) {
      owner->placments[index].expansion = eid{ex.index};
    };

  private:
    tool *owner;
    std::size_t index;
  };

  tool::placement_proxy create_placemnt(units::ui::position baseline,
                                        units::upixel height, sgid s, lsgid g) {
    placments.push_back({.baseline = baseline,
                         .height = height,
                         .semantic_group = s,
                         .logical_group = g});
    return tool::placement_proxy{this, placments.size() - 1};
  };
  tool::placement_proxy create_placemnt(units::ui::position baseline,
                                        units::upixel height, sgid s) {

    return create_placemnt(baseline, height, s, lsgid::null);
  };
  tool::placement_proxy create_placemnt(units::ui::position baseline,
                                        units::upixel height, lsgid g) {

    return create_placemnt(baseline, height, sgid::null, g);
  };
  tool::placement_proxy create_placemnt(units::ui::position baseline,
                                        units::upixel height) {
    return create_placemnt(baseline, height, sgid::null, lsgid::null);
  };

  // overload set to create_placement
  tool::expansion_proxy create_expansion() {
    expansions.push_back({});
    return tool::expansion_proxy{this, expansions.size() - 1};
  };

  present compile() {
    present_sequence res;

    for (auto &&p : placments) {
      // fill
      std::size_t begin{res.placement_glyph_sequence.size()};
      res.placement_glyph_sequence.append_range(p.seq);
      res.placements.push_back(
          {.range{.begin = begin, .end = res.placement_glyph_sequence.size()},
           .baseline = p.baseline,
           .height = p.height,
           .semantic_group = p.semantic_group,
           .logical_group = p.logical_group,
           .expansion = p.expansion});
    };

    for (auto &&ex : expansions) {
      std::size_t begin{res.expansion_glyph_sequence.size()};
      res.expansion_glyph_sequence.append_range(ex.seq);
      res.expansions.push_back(
          {.range{.begin = begin, .end = res.expansion_glyph_sequence.size()}});
    };

    return present{std::move(res)};
  };
  // TODO: void complie(allocator);
private:
  struct pdata {
    std::vector<placed> seq;
    units::ui::position baseline;
    units::upixel height;
    sgid semantic_group;
    lsgid logical_group;
    eid expansion;
  };

  struct edata {
    std::vector<gid> seq;
  };

  std::vector<pdata> placments;
  std::vector<edata> expansions;
};

}; // namespace iuic::text
