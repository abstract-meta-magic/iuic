// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.static__;
import std;
import :block.base;

namespace iuic::external {

struct static_block__ : block {
  general_context gctx;

  struct node {
    const declaration_segment *owner{nullptr};
    std::unique_ptr<resolution_segment> segment;
  };

  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retire() override {};

  constexpr virtual void die() override {};

  constexpr virtual resolution_segment *
  get_resolution_segment_for(const declaration_segment *ptr) override {
    auto it = std::find_if(segments.begin(), segments.end(),
                           [=](node &n) { return n.owner = ptr; });
    if (it != segments.end()) {
      return it->segment.get();
    } else {
      segments.push_back(node{
          ptr, std::unique_ptr<resolution_segment>{new resolution_segment{}}});
      return segments.front().segment.get();
    }
  };

  constexpr general_context *get_general_context() override { return &gctx; };

  std::vector<node> segments{};
};

template <typename T> constinit static_block__ static_block{};

}; // namespace iuic::external
