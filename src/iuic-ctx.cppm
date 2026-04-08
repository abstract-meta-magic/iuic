// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0

export module iuic.core:context;
import std;
import iuic.underlying;
import :environment.persist;
import :scheme.builder;
import :scheme.compute;
import :scheme.explorer;

export namespace iuic {

class context {
public: // api
  void make(units::ui::size viewport, scheme::builder_block_cpt auto &&call);

  context() {
    scheme = scheme::explorer_assign{blueprint.begin(), &tenv, &penv};
  };

private:
  // sync
  advance::pool adp;

  // explore
  scheme::blueprint blueprint;
  environment::tmp tenv{};
  environment::persist penv{adp};

public:
  scheme::explorer scheme{};
};

// Contex Template Impl
void context::make(units::ui::size vp, scheme::builder_block_cpt auto &&call) {

  // <<-----------------------<< advance
  adp.advance();

  // <<-----------------------<< begin

  scheme::director d{penv};

  auto [sketch, tenv_] = d.make(vp, std::forward<decltype(call)>(call));

  blueprint = scheme::compute(sketch, tenv_, penv);

  std::swap(tenv_, tenv);

  scheme = {blueprint.begin(), &tenv, &penv};
};

} // namespace iuic
