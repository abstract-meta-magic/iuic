// SPDX-License-Identifier: Apache-2.0

module iuic.asset;
import iuic.test;

namespace {
struct test : iuic::test::unit<test> {

  static constexpr std::string_view name{"iuic::asset::map"};

  void body(iuic::test::utils utils) {
    //
    iuic::asset::catalog catalog;

    catalog.make_definition("font.base");
    auto sti_1 = catalog.make_default_instance("font.base");
    auto dyi_1 = catalog.make_dynamic_instance("font.base");

    auto i_1 = catalog.get_instance(sti_1);
    auto i_2 = catalog.get_instance(dyi_1);
    if (i_1) {
      auto &i = i_1.value().get();

      utils.eq(i.of().of().get_name(), "font.base", "check name");
    };

    if (i_2) {
      auto &i = i_2.value().get();

      utils.eq(i.of().of().get_name(), "font.base", "check name");
    };

    catalog.remove_instance(sti_1);

    {
      auto i = catalog.get_instance(sti_1);
      utils.rq_false(i.has_value(), "after remove");
    }

    // catalog.make_static_instance("font.base@idle");
    // catalog.make_static_instance("font.base@hovered");
  }
} _{};

}; // namespace

extern "C++" int main() { return iuic::test::run(); };
