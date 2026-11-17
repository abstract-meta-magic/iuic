// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.profile;
import std;
export import :impl;

namespace iuic::profile {

template <typename T>
concept is_valid_profile_zone = requires(T obj) { T{"my zone"}; };

static_assert(is_valid_profile_zone<zone>, "Invalid zone inplementation");

}; // namespace iuic::profile
