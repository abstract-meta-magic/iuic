// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core;
import std;
export import iuic.underlying;
export import :base.color;
export import :layout.def;
export import :scheme;
export import :scheme.hitscan;
export import :scheme.builder;
export import :policy;
export import :layout.utils;
export import :text.font;
export import :text.token;
export import :text.present;
export import :text.fontset;
export import :style;
export import :machine.dispatcher;
export import :event;

export namespace iuic {

// base stye
constexpr style::decl def_style = []() {
  style::decl res{};

  res.shape.min_size = {units::percent{16}, units::percent{24}};

  res.shape.margin.top = units::percent{4};
  res.shape.margin.left = units::upixel{30};

  res.shape.border.top = units::upixel{20},
  res.shape.border.left = units::upixel{40};

  res.decoration.background = color::css::white{};

  return res;
}();

// future concept

class context {
public: // api
  void make(units::ui::size viewport, scheme::builder_block_cpt auto &&call);

private:
  void set_view_size(units::ui::size sz);

  void reset();
  // первичное вычисление своего размера
  void proccess_measure();
  // примерное расположение своих детей
  void proccess_arrange();
  // балансировка
  void proccess_position();
  // построение списка отрисовки на основе FTC
  void complite_scheme();

private:
  /* Концепт для поддержки __attribute__((weak))
  template<kernel_instance base,kernel_instance ...alt> context;

  auto instance__kernel__() {
    if constexpr (sizeof...(alt) > 0) {
      return base_kernel_instance ? base_kernel_instance()
                                  : ((alt ? alt() : nullptr) || ...);
    } else {
      return base_kernel_instance();
    }
  };
  */
  advance::pool adp;

  state::machine::dispatcher machine_dispatcher;

  text::present::aggregator tpa{};

public:
  text::fontslot font;

  scheme::explorer scheme{};
};

// Contex Template Impl
void context::make(units::ui::size vp, scheme::builder_block_cpt auto &&call) {

  // step 1
  reset();
  // <<-----------------------<< advance
  adp.advance();

  // <<-----------------------<< begin
  set_view_size(vp);

  // <<-----------------------<< execute machines
  machine_dispatcher.execute();

  // <<-----------------------<< make base

  // env.set_viewport_size(vp);

  // scheme::director d{env};

  // auto row_scheme = d.make(std::forward<decltype(call)>(call));

  // auto comp_scheme = scheme::computing(row_sheme, env);
};

} // namespace iuic
