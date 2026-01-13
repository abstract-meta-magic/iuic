// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:layout.frame.box;
import std;
import :layout.def;
export import :layout;

namespace iuic {

struct : frame_layout {
  measure_result measure(frame_measure_utils utils) const noexcept override {

    return {{0, 0}};
  };

  bool arrange(frame_arrange_utils utils) const noexcept override {
    return false;
  };
} box_layout;

}; // namespace iuic
