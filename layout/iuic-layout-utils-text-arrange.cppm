// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.layout:utils.text.arrange;
import :decl;
import :utils.base;

export namespace iuic::layout::arrange {

struct text_utils : public utils_base {
  text_utils(environment::tmp &tenv,
             std::span<const iuic::text::raw::token> tokens_,
             element_base_iterator it)
      : utils_base{tenv, it}, tokens{tokens_} {};

  const units::ui::area &self_area() const {
    return (tree::access_iterator{it})->arrange;
  };

  std::span<const iuic::text::raw::token> get_tokens() { return tokens; };

  std::span<const iuic::text::present::token>
  commit(const std::vector<iuic::text::present::token> &tks) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>(tks.size());

    if (mem.size() == tks.size()) {
      for (std::size_t i{0}, e{tks.size()}; i < e; ++i) {
        new (&mem[i]) iuic::text::present::token{tks[i]};
      };
    }

    return mem;
  };

  std::span<const iuic::text::present::token>
  commit(std::vector<iuic::text::present::token> &&tks) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>(tks.size());

    if (mem.size() == tks.size()) {
      for (std::size_t i{0}, e{tks.size()}; i < e; ++i) {
        new (&mem[i]) iuic::text::present::token{std::move(tks[i])};
      };
    }

    return mem;
  };

  std::span<const iuic::text::present::placement>
  commit(std::vector<iuic::text::present::placement> &&pls) {
    auto mem = tenv.memory.allocate<iuic::text::present::placement>(pls.size());

    if (mem.size() == pls.size()) {
      for (std::size_t i{0}, e{pls.size()}; i < e; ++i) {
        new (&mem[i]) iuic::text::present::placement{std::move(pls[i])};
      };
    }

    return mem;
  };

  std::span<const iuic::text::present::token>
  commit(iuic::text::present::token &&tk) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>();
    if (not mem.empty()) {
      new (&mem[0]) iuic::text::present::token{std::move(tk)};
    }
    return mem;
  };

  std::span<const iuic::text::present::token>
  commit(const iuic::text::present::token &tk) {
    auto mem = tenv.memory.allocate<iuic::text::present::token>();
    if (not mem.empty()) {
      new (&mem[0]) iuic::text::present::token{tk};
    }
    return mem;
  };

private:
  std::span<const iuic::text::raw::token> tokens;
};
}; // namespace iuic::layout::arrange
