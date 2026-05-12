// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.text.measure;
import :decl;
import :utils.base;

export namespace iuic::layout::measure {

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv,
             std::span<const iuic::text::raw::token> tokens_,
             element_base_iterator it)
      : utils_base{tenv, it}, tokens{tokens_} {};

  std::span<const iuic::text::raw::token> get_tokens() { return tokens; };

private:
  std::span<const iuic::text::raw::token> tokens;
};
}; // namespace iuic::layout::measure
