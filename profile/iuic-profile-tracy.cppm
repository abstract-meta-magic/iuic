// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module;
#define TRACY_ENABLE 1
#include <tracy/TracyC.h>

export module iuic.profile:impl;
import std;

namespace iuic::profile {

template <bool enable> struct basic_zone;

export struct zone {

public:
  zone(std::string_view name,
       std::source_location loc = std::source_location::current()) {

    const auto source = ___tracy_alloc_srcloc_name(
        loc.line(), loc.file_name(), std::strlen(loc.file_name()),
        loc.function_name(), std::strlen(loc.function_name()), name.data(),
        name.size(), 0);

    ctx = ___tracy_emit_zone_begin_alloc(source, 1);
  };

  ~zone() { ___tracy_emit_zone_end(ctx); }

private:
  TracyCZoneCtx ctx;
};
}; // namespace iuic::profile
