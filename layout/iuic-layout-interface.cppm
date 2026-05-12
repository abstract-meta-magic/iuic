// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:interface;
import :utils;

export namespace iuic::layout {
struct frame {
  constexpr virtual ~frame() {};

  constexpr frame() noexcept {};

  virtual std::optional<units::ui::adaptive::size>
  measure(measure::frame_utils utils) const = 0;

  virtual bool arrange(arrange::frame_utils utils) const = 0;
};

struct text {
  virtual ~text() {};
  virtual std::optional<units::ui::adaptive::size>
  measure(measure::text_utils utils) const = 0;

  virtual std::span<const iuic::text::present::token>
  arrange(arrange::text_utils utils) const = 0;
};
}; // namespace iuic::layout
