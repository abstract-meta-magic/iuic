// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.dynamic__;
import std;
import :block.base;

namespace iuic::external {

struct dynamic_block__ : block {
  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retire() override {};

  constexpr virtual void die() override {};

  constexpr virtual resolution_context
  get_resolution_context_for(const native_instance *) override {
    return {};
  };
};
} // namespace iuic::external
