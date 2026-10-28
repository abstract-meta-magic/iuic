// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.text;
import std;
import iuic.test;

namespace {

auto model = [](iuic::text::model::tool mt) {
  for (std::size_t i{0}; i < 128; ++i) {
    auto g = mt.create_glyph();
    auto gc = mt.create_context();

    iuic::text::codepoint c{static_cast<iuic::text::codepoint>(i)};

    auto gi = mt.create_glyph_info({.code = c});
    // mb meta

    mt.link_with(gc, g);
    mt.link_with(g, gi);
  };

  return mt.compile();
}({});

auto profile = [](iuic::text::model::profile::tool mpt) {
  auto cell_metrics =
      mpt.create_metrics_for_glyph({.width = 0.6, .baseline = 0.8});
  auto layout_metrics = mpt.create_metrics_for_context();

  for (auto g : model.glyph_range()) {
    mpt.link_with(g, cell_metrics);
  };

  for (auto c : model.context_range()) {
    mpt.link_with(c, layout_metrics);
  };

  return mpt.compile();
}({model});

struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::text[model.check]"};

  void body(iuic::test::utils){
      // model

  };
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
