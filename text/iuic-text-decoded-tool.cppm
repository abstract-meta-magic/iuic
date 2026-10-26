// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.text:decoded.tool;
import iuic.underlying;
import std;
import :base;
import :decoded;

namespace iuic::text {

struct decoded::tool {
  // In next versions
  // tool(decoded&) mute decoded
  // tool() create new decoded
  // In this version
  // decoded, after compile, immutabel

private:
  struct segment_proxy;

  struct expended_proxy {
    friend segment_proxy; // ??? REWORK
    void append(gcid c) { owner->expansions[index].sequence.push_back(c); };

    expended_proxy(tool *owner_, std::size_t index_)
        : owner{owner_}, index{index_} {};

  private:
    tool *owner;
    std::size_t index;
  };

  struct segment_proxy {
    void append(gcid c) { owner->segments[index].sequence.push_back(c); };

    void link_with(expended_proxy f) {
      owner->segments[index].expansion = eid{f.index};
    };

    segment_proxy(tool *owner_, std::size_t index_)
        : owner{owner_}, index{index_} {};

  private:
    tool *owner;
    std::size_t index;
  };

public:
  segment_proxy create_segment(sgid s, lsgid g) {
    segments.push_back({.group = g, .semantic = s, .expansion = eid::null});
    return segment_proxy{this, segments.size() - 1};
  };

  segment_proxy create_segment(sgid s) {
    return create_segment(s, lsgid::null);
  };

  segment_proxy create_segment(lsgid g) {
    return create_segment(sgid{sgid::null}, g);
  };

  segment_proxy create_segment() {
    return create_segment(sgid::null, lsgid::null);
  };

  expended_proxy create_expansion() {
    expansions.push_back({});
    return expended_proxy{this, expansions.size() - 1};
  };

  decoded compile() {
    // TODO : REFACTORING
    // This basic impl
    // NOT FINAL
    decoded_sequence res;

    for (auto &seg : segments) {
      auto begin = res.segments_context_sequence.size();
      res.segments_context_sequence.append_range(seg.sequence);
      res.segments.push_back(
          decoded_sequence::sdata{.group = seg.group,
                                  .semantic = seg.semantic,
                                  .expansion = seg.expansion,
                                  .begin = begin,
                                  .end = res.segments_context_sequence.size()});
    };

    for (auto &ex : expansions) {
      auto begin = res.expansions_context_sequence.size();
      res.expansions_context_sequence.append_range(ex.sequence);
      res.expansions.push_back(decoded_sequence::edata{
          .begin = begin,
          .end = res.expansions_context_sequence.size(),
      });
    };

    return decoded{std::move(res)};
  };

  // TODO: void complie(allocator);
private:
  struct sdata {
    std::vector<gcid> sequence;
    lsgid group;
    sgid semantic;
    eid expansion;
  };

  struct edata {
    std::vector<gcid> sequence;
  };

  std::vector<edata> expansions;
  std::vector<sdata> segments;
};

}; // namespace iuic::text
