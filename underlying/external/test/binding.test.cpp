// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

module iuic.underlying.external;
import std;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {
  static constexpr std::string_view name{"iuic::external[binding]"};

  void body(iuic::test::utils utils) {
    static constexpr iuic::external::type::decl test_type;
    static constexpr iuic::external::binding test_bind{test_type,
                                                       {"res://my/path"}};

    struct image_data {
      int x, y;
    };

    struct image_data_2 {
      int x, y;
    };

    constexpr auto test_inst_ct =
        test_bind.make_declaration<image_data{4, 4}>();

    static_assert(test_inst_ct.get_binding_type() == test_type,
                  "invalid binding type");

    static_assert(test_inst_ct.get_declaration_type() ==
                      iuic::erasure::type::from<image_data>(),
                  "invalid decl type");

    static_assert(test_inst_ct.get_uri().get_scheme() == "res://",
                  "invalid uri scheme");

    static_assert(test_inst_ct.get_uri().get_path() == "my/path",
                  "invalid uri path");

    utils.rq_true(
        test_inst_ct.try_visit_declaration([&](const image_data &data) {
          utils.eq(data.x, 4, "check value");
          utils.eq(data.y, 4, "check value");
        }),
        "check data ct-inst in rt");

    utils.rq_false(test_inst_ct.is_resolved(), "check resolve");

    // async ???
    test_inst_ct.resolve([](auto ctx) {
      // resolve
    });

    // check

    test_inst_ct.try_visit_resolution([](image_data_2 &data) {
      // do job
    });

    // check
  };
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
