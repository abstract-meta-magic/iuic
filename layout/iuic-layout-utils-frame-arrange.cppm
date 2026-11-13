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
    iterator::access ait{iterator::basic{it, el}};

    auto index = ait->is.index();

    bool is = std::holds_alternative<ui::element::measured>(ait->is);

    // make assert ??
    if (std::holds_alternative<ui::element::measured>(ait->is)) {
      ait->is = ui::element::placed{a};
    } else {
      //
      auto index = ait->is.index();
    }
  };

  void apply_element(base_iterator el, units::ui::area a,
                     units::clip_id clipzone) {
    iterator::access ait{iterator::basic{it, el}};

    // make assert ??
    if (std::holds_alternative<ui::element::measured>(ait->is)) {
      ait->is = ui::element::placed{a};
      ait->clip = clipzone;
    }
  };

  units::clip_id make_clipzone() {
    if (element_access_iterator ait{it};
        ait && ait->clip != units::clip_id::null) {
      return tenv.clip.make_clipzone(std::get<ui::element::placed>(ait->is),
                                     tenv.style.get(ait->sid), ait->clip);
    } else {
      return tenv.clip.make_clipzone(std::get<ui::element::placed>(ait->is),
                                     tenv.style.get(ait->sid));
    }
  };

  units::clip_id make_clipzone(const units::ui::area &);

  struct : utils::member_for<frame_utils> {
    const units::ui::adaptive::size &operator[](base_iterator el) {
      return std::get<ui::element::measured>(
          iterator::access{iterator::basic{self().it, el}}->is);
    };
  } measure{*this};

  const units::ui::area &self_area() const {
    return std::get<ui::element::placed>((element_access_iterator{it})->is);
  };
};
}; // namespace iuic::layout::arrange
