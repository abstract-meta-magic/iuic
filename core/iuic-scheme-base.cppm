// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.base;
import std;
import iuic.underlying;
import iuic.asset;
import iuic.text;
import iuic.layout;
import iuic.style;
import iuic.env;
import iuic.event;

import iuic.state; // tmp

namespace iuic::scheme {

export struct base {

protected:
  asset::catalog *assets;
  environment::tmp *tenv;
  environment::persist *penv;
};

export namespace iterator {

using base = layout::base_iterator;
using root = layout::root_iterator;
using sibling = layout::sibling_iterator;

// sibling childs_of(base it) { return iuic::tree::childs_of(it); };

}; // namespace iterator

}; // namespace iuic::scheme
