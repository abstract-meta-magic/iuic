// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
module;

export module iuic.profile:impl;
import std;

namespace iuic::profile {

template <bool enable> struct basic_zone;

export struct zone {

public:
  zone(std::string_view name,
       std::source_location loc = std::source_location::current()) {};
};
}; // namespace iuic::profile
