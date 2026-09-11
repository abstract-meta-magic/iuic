// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:context;
import std;
import iuic.underlying;
import iuic.env;
import :scheme.builder;
import :scheme.compute;
import :scheme.explorer;

export namespace iuic {

struct domain {
  domain(iuic::environment::domain *ptr_) : ptr{ptr_} {};

  template <typename T> auto get() { return ptr->get<T>(); }

private:
  iuic::environment::domain *ptr;
};

class context {
public: // api
  void make(units::ui::size viewport, scheme::builder_block_cpt auto &&call);

  void make(units::ui::size viewport, units::upixel segment,
            scheme::builder_block_cpt auto &&call);

  context() {
    scheme = scheme::explorer_assign{blueprint.begin(), &tenv, &penv};
  };

  void domain(std::invocable<domain> auto &&call) {
    call(iuic::domain{&denv});
  };

  void eval(std::invocable<iuic::scheme::explorer &> auto &&call) {
    // ... sync ??
    call(scheme);
  };

  // void eval(std::invocable<domai_proxy> auto &&);

private:
  // sync
  advance::pool adp;

  // explore
  scheme::blueprint blueprint;
  environment::tmp tenv{adp};
  environment::persist penv{adp};
  environment::domain denv;
  scheme::explorer scheme{};
};

void context::make(units::ui::size vp, scheme::builder_block_cpt auto &&call) {
  make(vp, units::upixel{8}, std::forward<decltype(call)>(call));
};

// Contex Template Impl
void context::make(units::ui::size vp, units::upixel seg,
                   scheme::builder_block_cpt auto &&call) {

  // <<-----------------------<< advance
  adp.advance();

  // <<-----------------------<< begin
  tenv.meta.viewport_size = vp;
  tenv.meta.segment_size = seg;

  scheme::director d{tenv, penv, denv};

  auto sketch = d.make(vp, std::forward<decltype(call)>(call));

  blueprint = scheme::compute(sketch, tenv, penv);

  scheme = {blueprint.begin(), &tenv, &penv};
};

} // namespace iuic
