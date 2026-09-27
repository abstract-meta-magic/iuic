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
    static constexpr iuic::external::binding test_bind{
        iuic::external::ct_binding<test_type, "res://my/path">{}};

    struct image_data {
      int x, y;
    };

    struct image_data_2 {
      int x, y;
    };

    struct image_shared_data {
      int x, y;
    };

    constexpr auto test_inst_ct =
        test_bind.make_ct_instance<image_data{4, 4}>();

    utils.eq(test_inst_ct.get_binding_type(), test_type,
             "invalid binding type");

    utils.eq(test_inst_ct.get_declaration_type(),
             iuic::erasure::type::from<image_data>(), "invalid decl type");

    utils.eq(test_inst_ct.get_uri().get_scheme(), "res://",
             "invalid uri scheme");

    utils.eq(test_inst_ct.get_uri().get_path(), "my/path", "invalid uri path");

    // RUNTIME
    utils.rq_true(
        test_inst_ct.try_visit_declaration([&](const image_data &data) {
          utils.eq(data.x, 4, "check value");
          utils.eq(data.y, 4, "check value");
          std::println("data has : {} , {}", data.x, data.y);
        }),
        "check data ct-inst in rt");

    // mb add async_resolve ???
    test_inst_ct.resolve([](iuic::external::resolution_context ctx) {
      auto shared = ctx.get_shared_object<image_shared_data>();

      if (not shared) {
        shared.try_emplace(12, 12);
        // or
        shared.try_set(image_shared_data{12, 12});
        shared.get().try_visit([](image_shared_data &) {});
        shared.reset();
      }

      auto res = ctx.get_resolution();
      if (not res) {
        res.try_emplace<image_data_2>(4, 4);
        // or
        res.try_set(image_data{4, 4});
        res.get().try_visit([](image_data_2 &) {});
        res.reset();
      }

      return true;
    });

    utils.eq(test_inst_ct.get_resolution_type(),
             iuic::erasure::type::from<image_data_2>(), "check type");

    utils.rq_true(test_inst_ct.try_visit_resolution([&](image_data_2 &data) {
      utils.eq(data.x, 4, "check value");
      utils.eq(data.y, 4, "check value");
    }),
                  "visit resolution");

    utils.rq_true(
        test_inst_ct.try_visit_shared_object([&](image_shared_data &data) {
          utils.eq(data.x, 12, "check value");
          utils.eq(data.y, 12, "check value");
        }),
        "visit shared");

    auto inst_2 = test_bind.make_instance();

    utils.rq_true(inst_2.try_visit_shared_object([&](image_shared_data &data) {
      utils.eq(data.x, 12, "check value");
      utils.eq(data.y, 12, "check value");
    }),
                  "visit shared");

    auto inst_3 = test_bind.make_instance(image_data{10, 10});

    utils.rq_true(inst_3.try_visit_declaration([&](const image_data &data) {
      utils.eq(data.x, 10, "check value");
      utils.eq(data.y, 10, "check value");
      std::println("data has : {} , {}", data.x, data.y);
    }),
                  "visit dynamic data");

    iuic::external::instance inst_4{};

    inst_4 = inst_3;

    utils.rq_true(inst_4.try_visit_declaration([&](const image_data &data) {
      utils.eq(data.x, 10, "check value");
      utils.eq(data.y, 10, "check value");
      std::println("data has : {} , {}", data.x, data.y);
    }),
                  "visit dynamic data");

    iuic::external::instance inst_5{};

    inst_5 = std::move(inst_4);

    utils.rq_true(inst_5.try_visit_declaration([&](const image_data &data) {
      utils.eq(data.x, 10, "check value");
      utils.eq(data.y, 10, "check value");
      std::println("data has : {} , {}", data.x, data.y);
    }),
                  "visit dynamic data");
  };
} _{};
}; // namespace

extern "C++" int main() { return iuic::test::run(); };
