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

    { // base place
      catalog.make_static_instance("font.base@idle");
      auto inst_id = catalog.get_static_instance("font.base@idle");
      auto inst_ = catalog.get_instance(inst_id);

      utils.rq_true((bool)inst_, "has value");
      if (inst_) {
        auto &inst = inst_.value().get();
        utils.eq(inst.of().of().get_name(), "font.base", "name");
      };

      catalog.remove_instance(inst_id);

      inst_id = catalog.get_static_instance("font.base@idle");

      utils.eq(inst_id, iuic::asset::invalid_id, "remove");
    }
    { // after remove place
      catalog.make_static_instance("font.base@idle");
      auto inst_id = catalog.get_static_instance("font.base@idle");
      auto inst_ = catalog.get_instance(inst_id);

      utils.rq_true((bool)inst_, "has value");
      if (inst_) {
        auto &inst = inst_.value().get();
        utils.eq(inst.of().of().get_name(), "font.base", "name");
      };

      catalog.remove_instance(inst_id);

      inst_id = catalog.get_static_instance("font.base@idle");

      utils.eq(inst_id, iuic::asset::invalid_id, "remove");
    }
  }
} _{};

struct test_2 : iuic::test::unit<test_2> {

  static constexpr std::string_view name{"iuic::asset::map"};

  void body(iuic::test::utils utils) {
    iuic::asset::catalog c;

    struct info {
      int x, y;
    };
    struct image {
      int x, y;
    };

    c.make_definition("icon.close");
    auto i = c.get_instance(c.make_default_instance("icon.close"));

    utils.rq_true(i.has_value(), "validate");

    if (i.has_value()) {
      auto &i_ = i.value().get();
      auto info_b = i_.get_bind<info>();
      auto image_b = i_.get_bind<image>();

      if (not info_b.has_value()) {
        info_b.emplace(4, 8);
      }

      if (not image_b.has_value()) {
        image_b.emplace(12, 20);
      }

      auto &ib = info_b.get();
      auto &imb = image_b.get();

      utils.eq(ib.x, 4, "value");
      utils.eq(ib.y, 8, "value");
      utils.eq(imb.x, 12, "value");
      utils.eq(imb.y, 20, "value");
    };

    // check static
    {
      c.make_static_instance("icon.close@idle");
      auto iid = c.get_static_instance("icon.close@idle");
      auto &inst = c.get_instance(iid).value().get();
      inst.get_bind<image>().emplace(7, 8);
    }
    {
      c.make_static_instance("icon.close@idle");
      auto iid = c.get_static_instance("icon.close@idle");
      auto &inst = c.get_instance(iid).value().get();
      auto &image_value = inst.get_bind<image>().get();

      utils.eq(image_value.x, 7, "value");
      utils.eq(image_value.y, 8, "value");

      { // tags
        static constexpr iuic::asset::tag gc;

        inst.add_tag(gc);
        utils.rq_true(inst.has_tag(gc), "has tag");
        inst.remove_tag(gc);
        utils.rq_false(inst.has_tag(gc), "remove tag");
        inst.add_tag(gc);
        utils.rq_true(inst.has_tag(gc), "has tag");

        utils.eq(inst.get_type(), iuic::asset::undefined, "type");
      }
    }

    {
      c.make_definition("icon.new");
      auto iid =
          c.make_static_instance("icon.new@idle", image{5, 10}, info{20, 40});
      auto &inst = c.get_instance(iid).value().get();

      utils.rq_true(inst.get_bind<image>().has_value(), "has value");
      utils.rq_true(inst.get_bind<info>().has_value(), "has value");

      if (inst.get_bind<image>().has_value()) {
        auto &image_value = inst.get_bind<image>().get();
        utils.eq(image_value.x, 5, "value");
        utils.eq(image_value.y, 10, "value");
      }

      if (inst.get_bind<info>().has_value()) {
        auto &info_value = inst.get_bind<info>().get();
        utils.eq(info_value.x, 20, "value");
        utils.eq(info_value.y, 40, "value");
      }
    }
  };

} _1{};
} // namespace

extern "C++" int main() { return iuic::test::run(); };
