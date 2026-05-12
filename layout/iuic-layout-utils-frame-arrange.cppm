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

    if (not ait->meta.is_applyed()) {
      struct : decltype(el) {
        std::size_t extract() { return self; };
      } ext{el};

      if (ext.extract() == 20) {
        void *heh = nullptr;
      };

      ait->arrange = a;
      ait->meta.set_applyed();
    } else {
      void *a = nullptr;
    };
  };

  // this is joke...BUT
  struct : utils::adv_member_for<frame_utils> {
    const units::ui::adaptive::size &operator[](base_iterator el) {
      return tree::access_iterator{tree::shift(self().it, el)}->measure;
    };
  } measure [[no_unique_address]];

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->arrange;
  };
};
}; // namespace iuic::layout::arrange
