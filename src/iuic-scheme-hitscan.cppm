// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:scheme.hitscan;
import iuic.underlying;
import :scheme;

export namespace iuic::scheme {

// концепт ???

namespace hitscan {
enum class code {
  none,
  pass,
  hit,
  // etc
};

struct result {
  units::uid uid;
  code code;
};

template <typename T>
concept cpt = requires(const explorer &ex, units::ui::position p, T obj) {
  { obj(ex, p) } -> std::convertible_to<std::set<result>>;
};
} // namespace hitscan
}; // namespace iuic::scheme
