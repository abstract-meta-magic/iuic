// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.core:layout.def;
import std;
import iuic.underlying;
import :text.font;

export namespace iuic {

struct request_size {
  units::ui::adaptive_unit width;
  units::ui::adaptive_unit height;
};

struct measure_request {
  request_size rq;
  enum type {
    base,
    // static
    // sticky
    // etc
  };
};

struct frame_measure_utils;
struct frame_arrange_utils;
struct frame_position_utils;

enum class measure_err {
  undefined,
  discarted,
  broken,
};

using measure_result = std::expected<measure_request, measure_err>;

struct frame_layout {
  virtual ~frame_layout() = default;

  virtual measure_result measure(frame_measure_utils) const noexcept = 0;

  virtual bool arrange(frame_arrange_utils) const noexcept = 0;
};

struct text_measure_utils;
struct text_arrange_utils;

struct text_layout {
  virtual ~text_layout() = default;

  virtual measure_result measure(text_measure_utils) const noexcept = 0;

  virtual text::glyph::sequence arrange(text_arrange_utils) const noexcept = 0;
};

}; // namespace iuic
