// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.compute;
import :scheme.base;
import :environment.persist;
import :environment.tmp;

namespace iuic::scheme {
blueprint compute(const sketch &sketch, environment::persist &penv,
                  environment::tmp &tenv) {
  //

  blueprint b{
      &tenv, &penv,
      sketch.tree.reflect([](const sketch::element &el) -> blueprint::element {
        return {.uid = el.uid, .sid = el.sid, .zorder = el.zorder};
      })};

  return b;
};
}; // namespace iuic::scheme
