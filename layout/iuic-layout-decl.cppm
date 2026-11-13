// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:decl;
import std;
import iuic.underlying;
import iuic.text;
import iuic.style;

export namespace iuic::layout {

struct frame;
struct text;
}; // namespace iuic::layout

namespace iuic::ui {

export struct element {
  struct maked {};
  struct discarded {};
  struct virtualized {};
  struct measured : units::ui::adaptive::size {};
  struct placed : units::ui::area {};
  std::variant<const layout::frame *, const layout::text *> type;
  std::variant<maked, measured, placed, discarded, virtualized> is;
  units::uid uid;
  style::sid sid;
  units::ui::order order;
  units::clip_id clip{units::clip_id::null};
  text::id text{text::id::null};
};

}; // namespace iuic::ui

export namespace iuic::layout {

using tree = iuic::tree::orderable<ui::element, std::uint16_t>;

using base_iterator =
    iuic::iterator::basic<iuic::tree::hierarchy::orderable<std::uint16_t>>;

using sibling_iterator =
    iuic::iterator::basic<iuic::tree::hierarchy::orderable<std::uint16_t>,
                          iuic::iterator::mode::sibling_t>;

using root_iterator =
    iuic::iterator::basic<iuic::tree::hierarchy::orderable<std::uint16_t>,
                          iuic::iterator::mode::root_t>;

using element_base_iterator =
    iuic::iterator::basic<iuic::tree::orderable<ui::element, std::uint16_t>>;

using element_access_iterator =
    iuic::iterator::access<iuic::tree::orderable<ui::element, std::uint16_t>>;

}; // namespace iuic::layout
