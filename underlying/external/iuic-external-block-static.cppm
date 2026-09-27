// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.static__;
import std;
import :block.base;
import :ct_binding;

namespace iuic::external {

template <ct_binding info> struct static_block__ : block {
  general_context gctx{&info.type, info.uri};

  struct node {
    const declaration_segment *owner{nullptr};
    std::unique_ptr<resolution_segment> segment;
  };

  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retain() override {};

  constexpr virtual void release() override {};

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
  constexpr const general_context *get_general_context() const override {
    return &gctx;
  };

  std::vector<node> segments{};
};

template <typename T, typename Info>
constinit static_block__<Info{}> static_block{};

}; // namespace iuic::external
