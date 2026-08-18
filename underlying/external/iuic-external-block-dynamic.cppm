// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.dynamic__;
import std;
import :block.base;

namespace iuic::external {

struct dynamic_block__ : block {
  general_context gctx;

  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retire() override {};

  constexpr virtual void die() override {};

  constexpr virtual resolution_segment *
  get_resolution_segment_for(const declaration_segment *ptr) override {
    if (auto it = segments.find(ptr); it != segments.end()) {
      return &it->second;
    } else {
      auto ins = segments.insert({ptr, {}});
      return &ins.first->second;
    }
  };

  constexpr general_context *get_general_context() override { return &gctx; };

  std::map<const declaration_segment *, resolution_segment> segments;
};
} // namespace iuic::external
