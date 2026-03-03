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

private:
  void set_view_size(units::ui::size sz) {
    penv.external.set_viewport_size(sz);
  };

  void reset() {};

private:
  advance::pool adp;

  environment::tmp tenv{};
  environment::persist penv{adp};

public:
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

  // <<-----------------------<< make base

  penv.external.set_viewport_size(vp);

  scheme::director d{penv};

  auto row_scheme = d.make(std::forward<decltype(call)>(call));

  auto comp_scheme = scheme::compute(row_scheme, penv, tenv);
};

} // namespace iuic
