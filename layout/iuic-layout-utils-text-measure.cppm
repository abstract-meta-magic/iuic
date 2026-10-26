// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.text.measure;
import :decl;
import :utils.base;

export namespace iuic::layout::measure {

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv, iuic::text::id text_,
             element_base_iterator it)
      : utils_base{tenv, it}, text{text_} {};

  iuic::text::decoded get_text() {
    return std::get<iuic::text::decoded>(tenv.text.get(text).value);
  };

  iuic::text::model::decoder get_decoder() {
    return tenv.text.get(text).decoder;
  };

  iuic::text::model::profile get_profile() {
    return tenv.text.get(text).profile;
  };

private:
  iuic::text::id text;
};
}; // namespace iuic::layout::measure
