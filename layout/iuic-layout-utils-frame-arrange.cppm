// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.frame.arrange;
import :decl;
import :utils.base;

export namespace iuic::layout::arrange {
struct frame_utils : public utils_base {
  frame_utils(environment::tmp &tenv, element_base_iterator it)
      : utils_base{tenv, it} {};

  void apply_element(base_iterator el, units::ui::area a) {
    tree::access_iterator ait{tree::shift(it, el)};

    auto &veu = *ait;

    // make assert ??
    if (not ait->meta.is_applyed()) {
      ait->arrange = a;
      ait->meta.set_applyed();
    }
  };

  void apply_element(base_iterator el, units::ui::area a,
                     units::clip_id clipzone) {
    tree::access_iterator ait{tree::shift(it, el)};

    auto &veu = *ait;

    // make assert ??
    if (not ait->meta.is_applyed()) {
      ait->arrange = a;
      ait->clipzone = clipzone;
      ait->meta.set_applyed();
    }
  };

  units::clip_id make_clipzone() {
    if (tree::access_iterator ait{it};
        ait &&
        ait->clipzone != units::clip_id{std::numeric_limits<
                             std::underlying_type_t<units::clip_id>>::max()}) {
      return tenv.clip.make_clipzone(ait->arrange, ait->style, ait->clipzone);
    } else {
      return tenv.clip.make_clipzone(ait->arrange, ait->style);
    }
  };

  units::clip_id make_clipzone(const units::ui::area &);

  struct : utils::member_for<frame_utils> {
    const units::ui::adaptive::size &operator[](base_iterator el) {
      return tree::access_iterator{tree::shift(self().it, el)}->measure;
    };
  } measure{*this};

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->arrange;
  };
};
}; // namespace iuic::layout::arrange
