// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.static__;
import std;
import :block.base;

namespace iuic::external {

struct static_block__ : block {
  constexpr virtual bool try_acquire() override { return true; };

  constexpr virtual void retire() override {};

  constexpr virtual void die() override {};

  constexpr virtual resolution_context
  get_resolution_context_for(const native_instance *) override {
    return {};
  };
};

template <typename T> constinit static_block__ static_block{};

}; // namespace iuic::external
