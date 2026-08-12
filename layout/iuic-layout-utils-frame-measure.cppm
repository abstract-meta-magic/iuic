// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.frame.measure;
import :utils.base;
import :decl;

export namespace iuic::layout::measure {
struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, element_base_iterator it)
      : utils_base{tenv, it} {};

  struct : utils::member_for<frame_utils> {
    const units::ui::adaptive::size &operator[](base_iterator el) {
      return tree::access_iterator{iuic::tree::shift(self().it, el)}->measure;
    };
  } measure{*this};
};
}; // namespace iuic::layout::measure
