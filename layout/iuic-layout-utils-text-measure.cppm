// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.text.measure;
import :decl;
import :utils.base;

export namespace iuic::layout::measure {

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv, text::decoded text_,
             element_base_iterator it)
      : utils_base{tenv, it}, text{text_} {};

  text::decoded get_text() { return text; };

private:
  text::decoded text;
};
}; // namespace iuic::layout::measure
