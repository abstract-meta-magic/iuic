// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.compute;
import :scheme.base;
import :environment.persist;
import :environment.tmp;
import :layout.utils;

namespace iuic::scheme {

blueprint compute(sketch &sketch, environment::tmp &tenv,
                  environment::persist &penv);

}; // namespace iuic::scheme
