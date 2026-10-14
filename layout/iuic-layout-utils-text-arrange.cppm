// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.text.arrange;
import :decl;
import :utils.base;

export namespace iuic::layout::arrange {

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv, text::decoded text_,
             element_base_iterator it)
      : utils_base{tenv, it}, text{text_} {};

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->arrange;
  };

  text::decoded get_text() { return text; };

private:
  text::decoded text;
};
}; // namespace iuic::layout::arrange
