// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.underlying.external:block.base;
import std;
import iuic.underlying.erasure;
import :type;
import :uri;

namespace iuic::external {

struct native_instance {
  type::value type;
  uri path;
  erasure::visited declaration{nullptr};
};

export struct resolution_context {
  //
  constexpr const uri &get_uri() const;
};

struct block {
  constexpr virtual ~block() {};

  constexpr virtual bool try_acquire() = 0;

  constexpr virtual void retire() = 0;

  constexpr virtual void die() = 0;

  constexpr virtual resolution_context
  get_resolution_context_for(const native_instance *) = 0;
};
}; // namespace iuic::external
