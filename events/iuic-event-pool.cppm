// Copyright (c) 2026 abstract-meta-magic and contributors
// SPDX-License-Identifier: Apache-2.0
export module iuic.event:pool;
import iuic.underlying;
import :decl;

export namespace iuic::event {

template <const channel &ch> struct pool : advance::interface {
  pool(advance::pool &p) { rebind(p); };
  // default
  pool();

  void push(package p) { pkgs.push_back(p); };

  std::span<package> list() { return pkgs; };

private:
  std::vector<package> pkgs;
};

}; // namespace iuic::event
