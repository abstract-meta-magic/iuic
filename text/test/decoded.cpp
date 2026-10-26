// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.text;
import std;
import iuic.test;

namespace {

struct test_1 : iuic::test::unit<test_1> {
  static constexpr std::string_view name{"iuic::text[decoded.fill]"};

  void body(iuic::test::utils utils) {
    iuic::text::decoded::tool dt;

    iuic::text::gcid c{44};

    auto seg = dt.create_segment();
    auto ex = dt.create_expansion();

    for (auto &&_ : std::views::iota(0, 2)) {
      seg.append(c);
    }

    seg = dt.create_segment();

    for (auto &&_ : std::views::iota(0, 4)) {
      seg.append(c);
    }

    seg = dt.create_segment();

    for (auto &&_ : std::views::iota(0, 8)) {
      seg.append(c);
    }

    iuic::text::decoded decoded = dt.compile();

    utils.eq(decoded.by_segments().size(), 3, "segment size");
    utils.eq(decoded.by_glyphs().size(), 14, "glyph size");
  };
} _1{};

struct test_2 : iuic::test::unit<test_2> {
  static constexpr std::string_view name{"iuic::text[decoded.logical]"};

  void body(iuic::test::utils utils) {
    iuic::text::decoded::tool dt;

    iuic::text::gcid c{44};
    iuic::text::lsgid g{7};

    auto seg = dt.create_segment(g);
    seg = dt.create_segment(g);

    auto decoded = dt.compile();

    utils.eq_then(decoded.by_segments().size(), 2, "segment size", [&]() {
      utils.eq(decoded.by_segments().at(0).get_logical_group(), g,
               "segment group");

      utils.eq(decoded.by_segments().at(0).get_logical_group(),
               decoded.by_segments().at(1).get_logical_group(),
               "segment group");
    });
  };
} _2{};

struct test_3 : iuic::test::unit<test_3> {
  static constexpr std::string_view name{"iuic::text[decoded.semantic]"};

  void body(iuic::test::utils utils) {
    iuic::text::decoded::tool dt;

    iuic::text::gcid c{44};
    iuic::text::sgid s{10};

    auto seg = dt.create_segment(s);
    seg = dt.create_segment(s);

    auto decoded = dt.compile();

    utils.eq_then(decoded.by_segments().size(), 2, "segment size", [&]() {
      utils.eq(decoded.by_segments().at(0).get_semantic_group(), s,
               "segment group");

      utils.eq(decoded.by_segments().at(0).get_semantic_group(),
               decoded.by_segments().at(1).get_semantic_group(),
               "segment group");
    });
  };
} _3{};

struct test_4 : iuic::test::unit<test_4> {
  static constexpr std::string_view name{"iuic::text[decoded.expansion]"};

  void body(iuic::test::utils utils) {
    iuic::text::decoded::tool dt;

    iuic::text::gcid c{44};
    iuic::text::sgid s{10};

    auto seg = dt.create_segment(s);

    for (auto &&_ : std::views::iota(0, 2)) {
      seg.append(c);
    }

    auto ex = dt.create_expansion();
    seg.link_with(ex);

    for (auto &&_ : std::views::iota(0, 8)) {
      ex.append(c);
    };

    auto decoded = dt.compile();

    utils.eq_then(decoded.by_segments().size(), 1, "segment size", [&]() {
      auto expansion = decoded.by_segments().at(0).get_expansion();
      utils.neq(expansion, iuic::text::eid::null, "expansion");
      // decoded.by_expansions().at(expansion);
    });
  };
} _4{};

}; // namespace

extern "C++" int main() { return iuic::test::run(); };
